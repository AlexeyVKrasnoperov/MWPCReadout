#ifndef RATEPLOT_H
#define RATEPLOT_H
#include <qwt_plot.h>
#include <qwt_plot_histogram.h>
#include <vector>
using namespace std;

class RatePlot : public QwtPlot
{
    Q_OBJECT
protected:
    QwtPlotHistogram *hist;
public:
    explicit RatePlot(QWidget *parent = 0);
    ~RatePlot(void);
    void Draw(uint id = -1, const QString & title = QString(), const vector<uint> * data = 0);
signals:

public slots:

};

#endif // RATEPLOT_H
