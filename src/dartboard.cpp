#include "dartboard.h"
#include "utils.h"
#include <QRegularExpression>
#include <QDebug>
#include <random>
#define _USE_MATH_DEFINES
#include <math.h>



DartBoard::DartBoard()
{
    initBoard();
}

void DartBoard::initBoard(){


     int cx = RADIUS_OUTER_DOUBLE_MM, cy=cx;

     double segment_angle = 360.0/20.0;
     double start_angle = 90.0 - segment_angle/2.0; // starting with 20 at the top

     fields.clear();
     availableScores.clear();

      // BULL'S EYE
     QPainterPath* double_bull = new QPainterPath();
     makePartialPiePart(double_bull,cx,cy,0,RADIUS_INNER_BULL_MM,0,360);
     QPainterPath* single_bull = new QPainterPath();
     makePartialPiePart(single_bull,cx,cy,RADIUS_INNER_BULL_MM,RADIUS_OUTER_BULL_MM ,0,360);

     fields.insert(QPair<int,BED>(25,SINGLE),single_bull);
     fields.insert(QPair<int,BED>(25,DOUBLE),double_bull);

     availableScores.insert(25,QPair<int,BED>(25,SINGLE));
     availableScores.insert(50,QPair<int,BED>(25,SINGLE));


     // SEGMENTS FROM 20 TO 1
     double angle=start_angle;
     for (int i=0;i<20;i++){

         QPainterPath* inner_single_bed = new QPainterPath();
         makePartialPiePart(inner_single_bed,cx,cy,RADIUS_OUTER_BULL_MM ,RADIUS_INNER_TRIPLE_MM,angle,segment_angle);
         QPainterPath* triple_bed = new QPainterPath();
         makePartialPiePart(triple_bed,cx,cy,RADIUS_INNER_TRIPLE_MM,RADIUS_OUTER_TRIPLE_MM,angle,segment_angle);
         QPainterPath* outer_single_bed = new QPainterPath();
         makePartialPiePart(outer_single_bed,cx,cy,RADIUS_OUTER_TRIPLE_MM,RADIUS_INNER_DOUBLE_MM,angle,segment_angle);
         QPainterPath* double_bed = new QPainterPath();
         makePartialPiePart(double_bed,cx,cy,RADIUS_INNER_DOUBLE_MM,RADIUS_OUTER_DOUBLE_MM,angle,segment_angle);

         fields.insert(QPair<int,BED>(SEGMENT_ORDER.at(i),INNER_SINGLE),inner_single_bed);
         fields.insert(QPair<int,BED>(SEGMENT_ORDER.at(i),TRIPLE),triple_bed);
         fields.insert(QPair<int,BED>(SEGMENT_ORDER.at(i),OUTER_SINGLE),outer_single_bed);
         fields.insert(QPair<int,BED>(SEGMENT_ORDER.at(i),DOUBLE),double_bed);


         availableScores.insert(SEGMENT_ORDER.at(i)  ,QPair<int,BED>(SEGMENT_ORDER.at(i),OUTER_SINGLE));
         availableScores.insert(SEGMENT_ORDER.at(i)*2,QPair<int,BED>(SEGMENT_ORDER.at(i),DOUBLE));
         availableScores.insert(SEGMENT_ORDER.at(i)*3,QPair<int,BED>(SEGMENT_ORDER.at(i),TRIPLE));


         angle+=segment_angle;
     }
}

void DartBoard::makePartialPiePart(QPainterPath* path, int cx, int cy, double radius1, double radius2, double start_angle, double angle){

  double s_a = (M_PI/180.0)*(double)start_angle;
  double a =(M_PI/180.0)*(double)angle;
  double x= ((double) cx) + (radius1*cos(s_a));
  double y= ((double) cy) - (radius1*sin(s_a));
  path->moveTo(x,y);
  path->arcTo(cx-radius1,cy-radius1,radius1*2,radius1*2,start_angle,angle);

  x=cx+ (radius2*cos(s_a+a));
  y=cy- (radius2*sin(s_a+a));
  path->lineTo(x,y);

  path->arcTo(cx-radius2,cy-radius2,radius2*2,radius2*2,start_angle+angle,-angle);
}


QPointF DartBoard::getCenterEstimate(QPair<int, DartBoard::BED> _field, double std_dev){

    int field = _field.first;
    BED bed = _field.second;
    QPainterPath* path = fields.value(QPair<int,BED>(field,bed));
    if(field==25 && bed==SINGLE && std_dev < 15.0){
        double halfring=((RADIUS_OUTER_BULL_MM - RADIUS_INNER_BULL_MM)/2)+RADIUS_INNER_BULL_MM;
        return path->toFillPolygon().boundingRect().center().toPoint()+QPointF(0,-halfring);
    }
    return path->toFillPolygon().boundingRect().center().toPoint();
}

QPointF DartBoard::aimDartAt(QPointF intendedLoc, double h_dev, double v_dev){


    std::random_device rd;
    std::default_random_engine generator(rd());
    std::normal_distribution<double> h_distribution(0.0,h_dev);
    std::normal_distribution<double> v_distribution(0.0,v_dev);

    double xo=h_distribution(generator);
    double yo=v_distribution(generator);

    QPointF hit(intendedLoc.x()+xo,intendedLoc.y()+yo);
    return hit;
}

QPair<int, DartBoard::BED> DartBoard::getThrowResult(QPointF location){
    QList< QPair<int,BED> > keys = fields.keys();
    for(int i=0;i<keys.length();i++){
        QPair<int,BED> field = keys.at(i);
        if(fields.value(field)->contains(location)){

            return field;
        }
    }
    return QPair<int,BED>(0,NONE);
}

int DartBoard::calcScore(QPair<int, BED> hitField){
    switch(hitField.second){
    case SINGLE:
    case OUTER_SINGLE:
    case INNER_SINGLE:
        return hitField.first;
    case DOUBLE:
        return hitField.first * 2;
    case TRIPLE:
        return hitField.first * 3;
    case NONE:
    default:
        return 0;
    }
}

QString DartBoard::getFieldName(QPair<int, BED> hitField, bool shortForm){

    QString name;
    QString score = QString::number(hitField.first).replace(25,"Bull");
    switch(hitField.second){
    case SINGLE:
    case OUTER_SINGLE:
    case INNER_SINGLE:
        name= shortForm ? score.replace("Bull","B") : score;
        break;
    case DOUBLE:
        name= shortForm ? "D"+score.replace("Bull","B") : "double "+score;
        break;
    case TRIPLE:
        name= shortForm ? "T"+score.replace("Bull","B") : "triple "+score;
        break;
    case NONE:
    default:
        name="0";
    }
    return name;
}

QMultiMap<int,QPair<int,DartBoard::BED> > DartBoard::getAvailableScores(){
    return availableScores;
}


QMap< QPair<int,DartBoard::BED> , QPainterPath* > DartBoard::getFields(){
    return fields;
}
