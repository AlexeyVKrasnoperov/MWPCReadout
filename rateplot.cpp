#include "rateplot.h"
#include "qwt_scale_draw.h"

RatePlot::RatePlot(QWidget *parent) :
    QwtPlot(parent)
{
    setAutoReplot(false);
    hist = new QwtPlotHistogram("");
    QColor color(Qt::blue);
    QPen pen(color);
    pen.setWidthF(1);
    hist->setPen(pen);
    hist->setBrush(QBrush(color));
    hist->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    hist->setStyle(QwtPlotHistogram::Outline);
    hist->attach(this);
    replot();
}

RatePlot::~RatePlot()
{
    hist->detach();
    delete hist;
}

void RatePlot::Draw(uint id, const QString & title, const vector<uint> * data)
{
    if( data != 0 )
    {
        if( id == uint(-1) )
        {
            setAxisTitle(QwtPlot::xBottom,"Wire");
            setAxisTitle(QwtPlot::yLeft,"Counts");

        }
        else
        {
            int hid = id & 0xF000;
            if( hid == 0x1000 )
            {
                setAxisTitle(QwtPlot::xBottom,"Time window");
                setAxisTitle(QwtPlot::yLeft,"Counts");
            }
            else if( hid == 0x2000 )
            {
                setAxisTitle(QwtPlot::xBottom,"Wire number");
                setAxisTitle(QwtPlot::yLeft,"Counts");
            }
            else
            {
                setAxisTitle(QwtPlot::xBottom,"Time, s");
                setAxisTitle(QwtPlot::yLeft,"Rate, Hz");
            }
        }
        //
        static QVector<QwtIntervalSample> sample;
        if( sample.size() != int(data->size()) )
            sample.resize(data->size());
        for( size_t i = 0; i < data->size(); i++ )
            sample[i] = QwtIntervalSample(data->at(i),i,i+1);
        hist->setSamples(sample);
        setTitle(title);
    }
    else
    {
        setAxisTitle(QwtPlot::xBottom,"");
        setAxisTitle(QwtPlot::yLeft,"");
        setTitle("");
        hist->setSamples(QVector<QwtIntervalSample>());
    }
    replot();
}

