#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

#include <QObject>
#include <QChronoTimer>
#include <QDebug>
#include <QVector>

using namespace std::chrono;

class datamanagement: public QObject
{
    Q_OBJECT
public:
    explicit datamanagement(QObject *parent = nullptr);

    //set
    void setVoltVal(double value)           {voltVal = value; qDebug()<< "전압: " <<voltVal;}
    void setInterval(nanoseconds time);
    void setVoltDialVal(int degValue)       {voltDegVal = degValue; qDebug()<< "전압 각도 : " << degValue;}
    void setFrequncy(double frequncyVal)    {frequncy = frequncyVal; qDebug() << "주파수 : " << frequncyVal;}
    void setCyclePerSecond(double cps);
    void setSamplePerCycle(int spc);
    void setTimeScale(int timeScaleVal);
    void setCurrVal(double value)           {currVal = value; qDebug() << "전류 : " <<currVal;}
    void setCurrDialVal(int degValue)       {currDegVal = degValue; qDebug() << "전류 각도 : " <<degValue;}


    //get
    double getVoltVal()         {return voltVal;}
    nanoseconds getInterval()   {return interval;}
    int getVoltDialVal()        {return voltDegVal;}
    double getFrequncy()        {return frequncy;}
    double getCyclePerSecond()  {return cpsVal;};
    int getSamplePerCycle()     {return spcVal;};
    int getTimeScale()          {return timeScale;}
    double getCurrVal()         {return currVal;}
    int getCurrDialVal()        {return currDegVal;}

    QVector<QPair<double, double>> data;

signals:
    void dataChanged(QVector<QPair<double, double>> data);

private slots:
    void intervalCapture();

private:
    QChronoTimer* t;

    double voltVal = 100;
    double currVal = 100;
    int voltDegVal = 0;
    int currDegVal = 0;
    double frequncy = 1;
    double cpsVal = 1;
    int spcVal = 1;
    int timeScale = 1;
    nanoseconds interval = nanoseconds(1000000000);
    nanoseconds currentTime = nanoseconds(0);
};

#endif // DATAMANAGEMENT_H
