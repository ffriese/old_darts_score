#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamefactory.h"
#include <QResizeEvent>
#include <QMessageBox>
#include <QSpinBox>
#include <QDebug>
#include <QSharedPointer>
#include <QFontDatabase>
#include <QBoxLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include "newgamedialog.h"
#include "manageplayersdialog.h"
#include "globalconfig.h"

#include "MQTTClient.h"

#define ADDRESS     "tcp://192.168.178.67:1883"
//#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "DartsScoringApp"
#define TOPIC       "board_coordinate"
#define QOS         1
#define TIMEOUT     10000L


volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    qDebug()<<"Message with token value "<< dt << "delivery confirmed";

    deliveredtoken = dt;
}


int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    qDebug()<<"message arrived!";
    QString mystr = QString::fromUtf8((char*)message->payload).left(message->payloadlen);
    qDebug()<<mystr;

    QJsonDocument doc = QJsonDocument::fromJson(mystr.toUtf8());
    QJsonObject jObject = doc.object();
    double x = jObject.take("x").toDouble();
    double y = jObject.take("y").toDouble();
    //convert the json object to variantmap
    QVariantMap mainMap = jObject.toVariantMap();
    qDebug()<<"["<<x<<","<<y<<"]";
    ((MainWindow*)context)->cvDart(QPointF(x,y));
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
void connlost(void *context, char *cause)
{
   qDebug()<<"Connection lost...";
}
/*void MainWindow::updateDart(const QMqttMessage &msg){
    qDebug()<<QString::fromStdString((msg.payload().toStdString()));
}*/


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

  /*  int id = QFontDatabase::addApplicationFont("../fonts/ScoreFont.ttf");
    QString family;
    if(id!=-1){
        family = QFontDatabase::applicationFontFamilies(id).at(0);
    }else{
        family="Arial";
    }*/

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    MQTTClient_setCallbacks(client, this, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
       // printf("Failed to connect, return code %d\n", rc);
        qDebug()<<"FAILED TO CONNECT TO MQTT BROKER AT"<<ADDRESS;
    }
    else{
        // printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
        //        "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
         MQTTClient_subscribe(client, TOPIC, QOS);
         qDebug()<<"CONNECTED TO MQTTBROKER AT"<<ADDRESS;
    }

/*
    sub = n.subscribe("/thrown_dart", 1000, &MainWindow::cvDart, this);
 // Use 1 thread
    spinner.reset(new ros::AsyncSpinner(1));
    spinner->start();
*/
    blocked = false;
    scoreFont = QFont("Arial",15);


    p.setColor(QPalette::Foreground,Qt::white);
    p.setColor(QPalette::Background,QColor(10,80,20));

    pn.setColor(QPalette::Foreground,Qt::black);
    pn.setColor(QPalette::Background,Qt::lightGray);

    highlightScore.setColor(QPalette::Foreground,Qt::green);
    highlightScore.setColor(QPalette::Background,Qt::black);

    this->setWindowIcon(QIcon(QCoreApplication::applicationDirPath()+"/../img/app.png"));



    ui->setupUi(this);



    database = DBManager::getInstance();

   // ui->mainToolBar->setVisible(false);
    ui->centralWidget->setStyleSheet("#centralWidget{background-image: url(:/images/img/wood.jpg)}");


    gameStatusLabel = new QLabel();

    toggleSoundsButton = new QPushButton();
    toggleSoundsButton->setCheckable(true);
    toggleSoundsButton->setChecked(false);
    toggleSoundsButton->setIcon(QIcon(":/images/img/sound_on.png"));
    connect(toggleSoundsButton,SIGNAL(toggled(bool)),this,SLOT(toggleSound(bool)));

    togglePaintDartsButton = new QPushButton();
    togglePaintDartsButton->setCheckable(true);
    togglePaintDartsButton->setChecked(true);
    togglePaintDartsButton->setIcon(QIcon(":/images/img/dart.png"));
    connect(togglePaintDartsButton,SIGNAL(toggled(bool)),this,SLOT(togglePaintDarts(bool)));

    ui->statusBar->addPermanentWidget(togglePaintDartsButton);
    ui->statusBar->addPermanentWidget(toggleSoundsButton);
    ui->statusBar->addPermanentWidget(gameStatusLabel);
    toggleSoundsButton->setMaximumWidth(toggleSoundsButton->height());
    togglePaintDartsButton->setMaximumWidth(togglePaintDartsButton->height());

    playerNameMapper = new QSignalMapper(this);
    playerNumMapper = new QSignalMapper(this);

    connect(playerNameMapper, SIGNAL(mapped(int)), this, SLOT(nameChanged(int)));
    connect(playerNumMapper, SIGNAL(mapped(int)), this, SLOT(playerNumChanged(int)));

    gameManager=new GameManager();

    connect(gameManager,SIGNAL(clearTakeRequest()),ui->dartboardWidget,SLOT(clearTake()));

    connect(gameManager,SIGNAL(signalGameStarted()),this,SLOT(initGame()));
    connect(gameManager,SIGNAL(signalPlayerUpdate()),this,SLOT(updatePlayers()));
    connect(gameManager,SIGNAL(signalGameActive(bool)),this,SLOT(setGameActive(bool)));

    connect(this,SIGNAL(signalPlayerChange(int,QSharedPointer<Player>)),gameManager,SLOT(requestPlayerChange(int,QSharedPointer<Player>)));
    connect(this,SIGNAL(signalPlayerNumChange(int,QList<QSharedPointer<Player>>)),gameManager,SLOT(changeNumOfPlayers(int,QList<QSharedPointer<Player>>)));

    connect(this,SIGNAL(newCvDart(QPointF,QPointF)),gameManager,SLOT(handleUserDart(QPointF,QPointF)));

    connect(ui->dartboardWidget,SIGNAL(newUserDart(QPointF,QPointF)),gameManager,SLOT(handleUserDart(QPointF,QPointF)));
    connect(ui->dartboardWidget,SIGNAL(requestAIDart(QPointF)),gameManager,SLOT(handleAIDartRequest(QPointF)));
    connect(ui->actionGenerate_Throw,SIGNAL(triggered()),gameManager,SLOT(throwAIDart()));
    connect(ui->actionClear_Darts,SIGNAL(triggered()),ui->dartboardWidget,SLOT(clearTake()));
    connect(ui->actionRestart_Game,SIGNAL(triggered()),gameManager,SLOT(resetGame()));
    connect(ui->actionNew_Game,SIGNAL(triggered()),this,SLOT(createNewGame()));
    connect(ui->actionManage_Players,SIGNAL(triggered()),this,SLOT(managePlayers()));
    connect(ui->actionStat_Viewer,SIGNAL(triggered(bool)),this,SLOT(openStatViewer()));

    QActionGroup* playerNumActions=new QActionGroup(this);
    playerNumActions->setExclusive(true);
    ui->action1->setActionGroup(playerNumActions);
    ui->action2->setActionGroup(playerNumActions);
    ui->action3->setActionGroup(playerNumActions);
    ui->action4->setActionGroup(playerNumActions);
    ui->action5->setActionGroup(playerNumActions);
    ui->action6->setActionGroup(playerNumActions);
    connect(ui->action1,SIGNAL(triggered(bool)),playerNumMapper,SLOT(map()));
    connect(ui->action2,SIGNAL(triggered(bool)),playerNumMapper,SLOT(map()));
    connect(ui->action3,SIGNAL(triggered(bool)),playerNumMapper,SLOT(map()));
    connect(ui->action4,SIGNAL(triggered(bool)),playerNumMapper,SLOT(map()));
    connect(ui->action5,SIGNAL(triggered(bool)),playerNumMapper,SLOT(map()));
    connect(ui->action6,SIGNAL(triggered(bool)),playerNumMapper,SLOT(map()));
    playerNumMapper->setMapping(ui->action1, 1);
    playerNumMapper->setMapping(ui->action2, 2);
    playerNumMapper->setMapping(ui->action3, 3);
    playerNumMapper->setMapping(ui->action4, 4);
    playerNumMapper->setMapping(ui->action5, 5);
    playerNumMapper->setMapping(ui->action6, 6);



    createNewGame(false);
    initGame();
  //  gameManager->resetGame();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
void MainWindow::cvDart(const geometry_msgs::Point::ConstPtr& msg){
    ROS_INFO_STREAM("RECEIVED ROS-COORDINATES: [" << msg->x <<","<< msg->y<<"]");
    emit newCvDart(QPointF(msg->x,msg->y),
                     QPointF());
}
*/




void MainWindow::toggleSound(bool off){
    if(off){
        toggleSoundsButton->setIcon(QIcon(":/images/img/sound_off.png"));
    }else{
        toggleSoundsButton->setIcon(QIcon(":/images/img/sound_on.png"));
    }
    GlobalConfig::SOUND_ENABLED = !off;
}

void MainWindow::togglePaintDarts(bool on){
    GlobalConfig::PAINT_DARTS = on;
    ui->dartboardWidget->repaint();
}

void MainWindow::managePlayers(){
    ManagePlayersDialog mpd;
    mpd.setWindowTitle("Manage Players");
    mpd.exec();

    for(PlayerWidget* pwid: playerWidgets){
        pwid->updatePlayers();
    }
    db_players.clear();
    dbPlayerOrder = database->getPlayers(true);

    for(QSharedPointer<Player> pl: dbPlayerOrder){
        db_players.insert(pl->getName(),pl);
    }

}

void MainWindow::createNewGame(bool show_diag){
    if(show_diag){
        NewGameDialog ngd;
        ngd.setWindowTitle("New Game");
        ngd.setSelectedGame(gameManager->getGame());
        connect(&ngd,SIGNAL(gameCreated(QSharedPointer<Game>)),gameManager,SLOT(setGame(QSharedPointer<Game>)));
        ngd.exec();
    }else{
        gameManager->newGame("301", {Game::VARIANT::X01_SINGLE_IN,Game::VARIANT::X01_SINGLE_OUT});
    }
    gameStatusLabel->setText(" "+gameManager->getStatusString()+" ");
}

void MainWindow::openStatViewer(){
    if(statViewer.isNull() || !statViewer->isVisible()){
        statViewer.reset(new StatViewer());
        statViewer->setWindowTitle("Statistics");
        statViewer->setVisible(true);
        statViewer->show();
    }
}


void MainWindow::fakeResize(){

    //TODO: do this the right way
    this->resize(this->geometry().width()+1, this->geometry().height());
    this->resize(this->geometry().width()-1, this->geometry().height());
}

void MainWindow::setGameActive(bool active){


    for(int i=0;i<playerWidgets.count();i++){
        playerWidgets.at(i)->setActive(active);
    }

    ui->menuSet_Players->setEnabled(!active);
    ui->actionAdd_New_Player->setEnabled(!active);

    if(active){
        ui->actionRestart_Game->setText("Restart Game");
    }else{
        ui->actionRestart_Game->setText("Start Game");
    }

}

void MainWindow::initUi(){

    qDebug()<<"initUI called";
    switch(gameManager->getPlayers().count()){
    case 1:
        ui->action1->setChecked(true);
        break;
    case 2:
        ui->action2->setChecked(true);
        break;
    case 3:
        ui->action3->setChecked(true);
        break;
    case 4:
        ui->action4->setChecked(true);
        break;
    case 5:
        ui->action5->setChecked(true);
        break;
    case 6:
        ui->action6->setChecked(true);
        break;
    default:
        break;
    }

    //qDeleteAll(playerFrames);
    for(QFrame* frame: playerFrames){
        frame->deleteLater();
    }

    for(PlayerWidget* wid: playerWidgets){
        disconnect(wid,SIGNAL(takeSelected(QSharedPointer<Take>)),ui->dartboardWidget,SLOT(setTake(QSharedPointer<Take>)));
        disconnect(wid,SIGNAL(playerSelected(QString)),playerNameMapper,SLOT(map()));
        wid->deleteLater();
    }

/*

    for(QListWidget* takewidget: playerTakes){
        takewidget->deleteLater();
        disconnect(takewidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(handleTakeSelection(QListWidgetItem*)));
        disconnect(takewidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(listMenuRequested(QPoint)));
    }
*/
    playerFrames.clear();
    playerWidgets.clear();

    db_players.clear();
    dbPlayerOrder = database->getPlayers();

    for(QSharedPointer<Player> pl: dbPlayerOrder){
        db_players.insert(pl->getName(),pl);
    }




    for(int i=0;i<gameManager->getPlayers().count();i++){
        QFrame* frame = new QFrame(centralWidget());
        playerFrames.append(frame);
        QVBoxLayout* boxL = new QVBoxLayout();
        boxL->setMargin(2);
        frame->setLayout(boxL);

        PlayerWidget* p_wid = new PlayerWidget(frame, gameManager->getGame(), dbPlayerOrder, gameManager->getPlayers().at(i), i);
        frame->layout()->addWidget(p_wid);
        playerWidgets.append(p_wid);

        p_wid->setVisible(true);
        frame->setVisible(true);
        qDebug()<<"created"<<p_wid<<p_wid->isVisible();

        connect(p_wid,SIGNAL(takeSelected(QSharedPointer<Take>)),ui->dartboardWidget,SLOT(setTake(QSharedPointer<Take>)));
        connect(p_wid,SIGNAL(playerSelected(QString)),playerNameMapper,SLOT(map()));
        playerNameMapper->setMapping(p_wid, i);
    }


    fakeResize();


    updatePlayers();
}

void MainWindow::nameChanged(int index){
    QString name = playerWidgets.at(index)->getCurrentPlayerName();
    if(gameManager->getPlayers().at(index)->getName() == name){
        return;
    }

    if(db_players.contains(name)){
        qDebug()<<"change "<<gameManager->getPlayers().at(index)->getName()<<"to"<<name;
        emit signalPlayerChange(index,db_players.value(name));
    }else{
        qDebug()<<"error! Player "<<name<<"not in database!";
    }

}

void MainWindow::playerNumChanged(int index){
    emit signalPlayerNumChange(index,dbPlayerOrder);
}


/*
void MainWindow::listMenuRequested(QPoint p){

    for(QListWidget* list: playerTakes){
        list->clearSelection();
    }
    QListWidget *list = qobject_cast<QListWidget *>(QObject::sender());
    if(list)
    {
      // sender is a QListWidget
        QPoint globalPos = list->mapToGlobal(p); // Map the global position to the userlist
        QModelIndex t = list->indexAt(p);
        if(t.row()<0){
            return; //no item selected
        }
        list->item(t.row())->setSelected(true); // even a right click will select the item
        QMenu* userListMenu = new QMenu(this);
        QAction* editAction = new QAction("edit",userListMenu);
        userListMenu->addAction(editAction);
        QAction* result = userListMenu->exec(globalPos);
        qDebug()<<result;
        userListMenu->deleteLater();
        editAction->deleteLater();
    }


}
*/
void MainWindow::initGame(){
    initUi();
}

void MainWindow::resizeEvent(QResizeEvent *event){


    int w = event->size().width();
    int h = event->size().height();

    int x = 0;
    int y = 0; ui->menuBar->visibleRegion().boundingRect().height();//+ ui->pushButton->height();

    h-= ui->menuBar->height();
    h-= ui->statusBar->height();
    h-= ui->mainToolBar->height();


    int side = std::min(w*4/7,h);

    int _w = (w-side)/2;
    ui->dartboardWidget->setGeometry(_w,y,side,h);

    if(gameManager->getPlayers().count()>playerFrames.size() || gameManager->getPlayers().count()<1){
        return;
    }
    int rows = ceil(((double)gameManager->getPlayers().count()) / 2) ;
    int _h = h / rows;

    for(int i=0;i<gameManager->getPlayers().count();i++){
        int _x = (i%2==0) ? 0 : _w+side;
        int _y = floor(((double)i)/2)*_h +y;
        playerFrames.at(i)->setGeometry(_x,_y,_w,_h);

    }


}

bool MainWindow::saveDialogYes(QSharedPointer<Player> winner){
    QMessageBox::StandardButton reply;

      reply = QMessageBox::question(this, "Congratulations "+winner->getName()+"!",
                                    "Do you want to save the game to the database?\n"
                                          "AI scores might not be included."
                                    "\nNOT YET IMPLEMENTED!",
                                    QMessageBox::Yes|QMessageBox::No);
      return reply==QMessageBox::Yes;
}

void MainWindow::updatePlayers(){


    for(PlayerWidget* p: playerWidgets){
        p->update();
    }

    // DRAW DARTS ON BOARD (SELECT TAKE TO BE DISPLAYED)
    if(gameManager->isActive()){
        if(gameManager->getCurrentPlayer()->getDartsLeft()==3){
            if(gameManager->getLastPlayer()->getTakes().count()>0){
                ui->dartboardWidget->setTake(gameManager->getLastPlayer()->getTakes().last());
            }
        }else{
            ui->dartboardWidget->setTake(gameManager->getCurrentPlayer()->getCurrentTake());
        }
    }



    int winner = gameManager->getLegWinner();

    for(int i=0;i<playerWidgets.count();i++){
       if(i==winner){
           qDebug()<<"found leg winner!";
           playerFrames.at(i)->setPalette(Qt::green);
           ui->dartboardWidget->setTake(gameManager->getPlayers().at(i)->getTakes().last());
       }else{
           playerFrames.at(i)->setPalette(p);
       }
    }

    if(gameManager->getWinner()!=-1){
      //  gameManager->getGame()->block();
        blocked = true;
        QString err;
        DBManager::getInstance()->saveGame(gameManager->getGame(),err);

        blocked = false;
        if(err!=""){
            QMessageBox msgBox;
            msgBox.setText(err);
            msgBox.exec();
        }
      //  gameManager->getGame()->unblock();
       /*  if(saveDialogYes(gameManager->getPlayers().at(gameManager->getWinner()))){
             qDebug()<<"SAVE!";
             DBManager::getInstance()->saveGame(gameManager->getGame());
             //TODO: get nextGameId from database ONLY at game restart/new game. check if current game has been saved already
         }*/
    }


}




void MainWindow::keyPressEvent(QKeyEvent *event){

    if(blocked){
        event->ignore();
        return;
    }
    switch(event->key()){
    case Qt::Key_Backspace:
        gameManager->undoLastDart();
        break;
    case Qt::Key_Return:
        if(!gameManager->inProgress()){
            gameManager->continueGame();
        }

        break;
    case Qt::Key_B:
        if(gameManager->isActive()){
            gameManager->addBounceOut();
        }
    case Qt::Key_G:
        if(gameManager->inProgress()){
            gameManager->throwAIDart();
        }

    default:
        break;
    }
}
