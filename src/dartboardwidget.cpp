#include "dartboardwidget.h"
#include <QPainter>
#include <QFrame>
#define _USE_MATH_DEFINES
#include <math.h>
#include <QDebug>
#include <QMouseEvent>
#include <QPair>
#include <QFontDatabase>
#include <QApplication>
#include <QVector2D>
#include <QtSvg/QSvgGenerator>
#include "utils.h"
#include "globalconfig.h"

DartBoardWidget::DartBoardWidget(QWidget *parent) : QWidget(parent)
{
    this->setCursor(Qt::CrossCursor);
    int id = QFontDatabase::addApplicationFont(":/fonts/fonts/WireOne.ttf");
    QString family;
    if(id!=-1){
        family = QFontDatabase::applicationFontFamilies(id).at(0);
    }else{
        family="Arial";
    }
    wireFont = QFont(family,20);
    dartBoard = DartBoard::getInstance();
    paths=dartBoard->getFields();
    currentTake.reset();
}

void DartBoardWidget::mousePressEvent(QMouseEvent * event){
    switch(event->button()){
    case Qt::LeftButton:
        break;
    case Qt::RightButton:
        intendedLoc=QPoint(event->x(),event->y());
        break;
    case Qt::MidButton:
        break;
    default:
        event->ignore();
        return;
    }
}

void DartBoardWidget::mouseReleaseEvent(QMouseEvent *event){

    if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) == true) {
        qDebug()<<"CTRL PRESSED!";
        QPointF click = mouseCoordsToBoardCoords(event->x(),event->y());
        if(currentTake!=NULL){
            for(QSharedPointer<Dart> dart: currentTake->getDarts()){
                QVector2D distVec(click-dart->getExactLocation());
                if(distVec.length()<=3.0){
                    qDebug() << "FOUND DART:" << dartBoard->getFieldName(dartBoard->getThrowResult(dart->getExactLocation()));
                }
            }
        }

    }else{
        switch(event->button()){
        case Qt::LeftButton:
            emit newUserDart(mouseCoordsToBoardCoords(event->x(),event->y()),
                             QPointF());
            event->accept();
            break;
        case Qt::RightButton:
            emit newUserDart(mouseCoordsToBoardCoords(event->x(),event->y()),
                             mouseCoordsToBoardCoords(intendedLoc.x(),intendedLoc.y()));
            event->accept();
            break;
        case Qt::MidButton:
            emit requestAIDart(mouseCoordsToBoardCoords(event->x(),event->y()));
            event->accept();
            break;
        default:
            event->ignore();
            return;
        }
    }



}
QPointF DartBoardWidget::mouseCoordsToBoardCoords(int x, int y){

    double _x = ((double) x) / scale;
    double _y = ((double) y) / scale;

    return QPointF(_x-OFFSET_FROM_ORIGIN_MM,_y-OFFSET_FROM_ORIGIN_MM);
}


void DartBoardWidget::setTake(QSharedPointer<Take> take){
    currentTake=take;
    repaint();
}

void DartBoardWidget::clearTake(){
    currentTake.reset();
    repaint();
}

void DartBoardWidget::paintEvent(QPaintEvent *){


    QColor black_bg = QColor(10,10,10);

    scale=((double) this->width()) / ((RADIUS_OUTER_DOUBLE_MM + OFFSET_FROM_ORIGIN_MM) * 2.0);
    QPainter painter(this);
   /* QSvgGenerator generator;
        generator.setFileName("tst.svg");
        generator.setSize(QSize(450*scale, 450*scale));
        generator.setViewBox(QRect(0, 0, 450*scale, 450*scale));
        generator.setTitle(tr("SVG Generator Example Drawing"));
        generator.setDescription(tr("An SVG drawing created by the SVG Generator "
                                    "Example provided with Qt."));

    QPainter painter;
        painter.begin(&generator);
      */
    painter.setRenderHints(QPainter::Antialiasing |QPainter::SmoothPixmapTransform);

   // QTransform transform;
  //  transform.scale(scale,scale);
  //  painter.setTransform(transform);
    painter.scale(scale,scale);



    double off=OFFSET_FROM_ORIGIN_MM;
    QPointF center = QPointF(RADIUS_OUTER_DOUBLE_MM,RADIUS_OUTER_DOUBLE_MM);
    QPoint offset(off,off);

    // DRAW BLACK BOARD BACKGROUND

    //SHADOW
    painter.save();
    painter.setBrush(QBrush(QColor(20,20,20,100)));
    painter.setPen(QColor(20,20,20,0));
    painter.drawEllipse(center+offset+QPointF(6,7),RADIUS_OUTER_DOUBLE_MM*1.22, RADIUS_OUTER_DOUBLE_MM*1.22);
    painter.restore();


    painter.save();
    painter.setBrush(QBrush(black_bg));
    painter.drawEllipse(center+offset,RADIUS_OUTER_DOUBLE_MM*1.22, RADIUS_OUTER_DOUBLE_MM*1.22);
    painter.restore();

    //DRAW WIRE-RING (WITHOUT NUMBERS)
    painter.setPen(QColor(255,255,255));
    painter.drawEllipse(center+offset,RADIUS_OUTER_DOUBLE_MM*1.188, RADIUS_OUTER_DOUBLE_MM*1.188);


    QColor green = QColor(0,131,73);
    QColor red = QColor(186,21,50);
    QPen pen = QPen(QColor(200,200,200));


    QList< QPair<int,DartBoard::BED> > keys = paths.keys();
    for(int i=0;i<keys.length();i++){
        QPair<int,DartBoard::BED> field = keys.at(i);
        QColor color,single_color;

        int idx = SEGMENT_ORDER.indexOf(field.first);
        DartBoard::BED bed = field.second;

        if(idx==-1){ // BULLS EYE
            color=red;
            single_color=green;
        }else if(idx%2==0){
            color=red;
            single_color=black_bg;
        }else{
            color=green;
            single_color=QColor(225,208,182);
        }
        switch(bed){
        case DartBoard::SINGLE:
        case DartBoard::OUTER_SINGLE:
        case DartBoard::INNER_SINGLE:
            color = single_color;
            break;
        case DartBoard::TRIPLE:
            //DRAW NUMBERS
            QPointF tripleCenter = paths.value(field)->boundingRect().center();
            QPointF textPos = tripleCenter - center;

            bool new_font = true;

            if(new_font){ // NEW FONT
                if(idx<6 || idx > 14){
                    textPos *= 1.85;
                }else{
                    textPos *= 1.93;
                }

            }else{ // OLD FONT
               textPos *= 1.9;
            }


            textPos += center+offset;
            painter.save();
            painter.translate(textPos.x(),textPos.y());
            float angle = atan2(center.y()+off - textPos.y(), center.x()+off - textPos.x());
            angle = angle *180/M_PI; //convert to degrees
            int up = -90;

            if(angle<-1) //11 and 6 are still drawn normally, everything below flips
                up+=180;
            painter.rotate((angle) + up);
            painter.setFont(wireFont);
            painter.setPen(Qt::white);

            painter.setRenderHints(QPainter::Antialiasing |QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

            if(new_font){ //NEW FONT
                QPainterPath textPath;

                int x0 = angle<-1? -off/6  : -off/6;
                int y0 = angle<-1? off/6 : off/6;

                textPath.addText(x0,y0, wireFont, QString::number(field.first));
                painter.drawPath(textPath);

            }else{ //OLD FONT
                painter.drawText(-off/2,-off/2,off,off,Qt::AlignCenter | Qt::AlignHCenter, QString::number(field.first));
            }








            painter.restore();

            break;
        }


        painter.fillPath((paths.value(field)->translated(OFFSET_FROM_ORIGIN_MM,OFFSET_FROM_ORIGIN_MM)),QBrush(color));

    }





    // DRAW WIRE
    if(floor(scale)>0){
        pen.setWidthF(scale/2);
        painter.setPen(pen);

        QList< QPair<int,DartBoard::BED> > keys = paths.keys();
        for(int i=0;i<keys.length();i++){
            QPair<int,DartBoard::BED> field = keys.at(i);
            if(field.first<25){ // BULL'S EYE WIRE IS DRAWN AS A CIRCLE LATER ON
                painter.drawPath(paths.value(field)->translated(OFFSET_FROM_ORIGIN_MM,OFFSET_FROM_ORIGIN_MM));
            }
        }

       /*
        painter.drawEllipse(RADIUS_OUTER_DOUBLE_MM-RADIUS_INNER_BULL_MM,
                            RADIUS_OUTER_DOUBLE_MM-RADIUS_INNER_BULL_MM,
                            RADIUS_INNER_BULL_MM*2,RADIUS_INNER_BULL_MM*2);
        /*/
    }

    //DRAW THROWS
    painter.setPen(QPen(QColor(0,255,155)));
    if(!currentTake.isNull()){
        for(QSharedPointer<Dart> dart: currentTake->getDarts()){
            QPointF target=dart->getIntendedLoctaion();
            QPointF result=dart->getExactLocation();
            if(target.isNull()){
                painter.setBrush(QBrush(QColor(0,0,255)));
                painter.drawEllipse(result+QPointF(OFFSET_FROM_ORIGIN_MM,OFFSET_FROM_ORIGIN_MM),1.8,1.8);
            }else{
                painter.setBrush(QBrush(QColor(255,0,0)));
                painter.drawEllipse(target+QPointF(OFFSET_FROM_ORIGIN_MM,OFFSET_FROM_ORIGIN_MM),1.8,1.8);
                painter.setBrush(QBrush(QColor(0,0,255)));
                painter.drawEllipse(result+QPointF(OFFSET_FROM_ORIGIN_MM,OFFSET_FROM_ORIGIN_MM),1.8,1.8);
                painter.drawLine(target+QPointF(OFFSET_FROM_ORIGIN_MM,OFFSET_FROM_ORIGIN_MM),result+QPointF(OFFSET_FROM_ORIGIN_MM,OFFSET_FROM_ORIGIN_MM));
            }
            if(GlobalConfig::PAINT_DARTS){
                painter.drawImage(QRectF(result.x()+OFFSET_FROM_ORIGIN_MM-(55.0/2.5),
                                         result.y()+OFFSET_FROM_ORIGIN_MM-(72.0/2.5),
                                         267.0/2.5,
                                         307.0/2.5),
                                  QImage(":images/img/hit.png"));
            }
        }
    }


   // painter.end();
}
