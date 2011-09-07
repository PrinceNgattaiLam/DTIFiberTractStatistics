#ifndef DEF_PLOTWINDOW
#define DEF_PLOTWINDOW


#include <QWidget>
#include <QApplication>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPen>
#include <QSlider>
#include <QLCDNumber>
#include <QTextEdit>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include "GlobalFunction.h"
#include "DTIAtlasFiberAnalyzerguiwindow.h"

typedef QVector<QVector<double> > qv2double;
typedef QVector<qv2double> qv3double;
typedef std::vector<std::string> vstring;
class DTIAtlasFiberAnalyzerguiwindow;

/********************************************************************************
 *PlotWindow Class : 
 *		Display curves of checked parameters for each selected cases.
 *	 Besides, plane informations are diplayed. 
 ********************************************************************************/

class PlotWindow : public QWidget
{
	Q_OBJECT
			
	public:
		PlotWindow(QVector<qv3double> casedata, QVector<qv3double> atlasdata, QVector<qv3double> statdata, std::string parameters="", DTIAtlasFiberAnalyzerguiwindow* parent=0);
		void InitWidget();
		void CreateCaseStyle();
		void CreateAtlasStyle();
		void CreateStatStyle();
		void CreateParameterButtons();
		void CreateCaseBoxes();
		void CreateAtlasBoxes();
		void CreateStatBoxes();
		void CreateFiberButtons();
		void CreateCurves();
		int GetCheckedParameter();
		std::vector<int> GetCheckedCase();
		std::vector<int> GetCheckedAtlas();
		std::vector<int> GetCheckedStat();
		int GetCheckedFiber();
		void Plotting(QVector<qv3double> data, std::string type);
		void CreateInfoLabel();
		void ChangeInfoLabel(vstring);
		void setSliderLcd();
		void ComputeCorr(QVector<qv3double> casedata, QVector<qv3double> statdata);
		void InitCorrText();
		bool getError(){return m_Error;}

		
	public slots:
		void setCurveVisible();
		void setPenWidth(int);
		void ColorCorr();
		void DecolorCorr();
		void ApplyTh(int);
		
		
	private:
		QHBoxLayout* m_MainLayout;
		QVBoxLayout* m_VLayout;
		QVBoxLayout* m_InfoLayout;
		QHBoxLayout* m_OptionLayout;
		QVBoxLayout* m_ParameterLayout;
		QVBoxLayout* m_CheckBoxLayout;
		QGridLayout* m_CaseLayout;
		QGridLayout* m_AtlasLayout;
		QGridLayout* m_StatLayout;
		QVBoxLayout* m_FiberLayout;
		QGridLayout* m_PixelGridLayout;
		QGridLayout* m_CorrLayout;
		std::vector <QLabel*> m_InfoLabel;
		std::vector <QRadioButton*> m_ParameterButtons;
		std::vector <QRadioButton*> m_FiberButtons;
		std::vector <QCheckBox*> m_CaseBoxes;
		std::vector <QCheckBox*> m_AtlasBoxes;
		std::vector <QCheckBox*> m_StatBoxes;
		std::vector <QLabel*> m_CaseLabel;
		std::vector <QLabel*> m_AtlasLabel;
		std::vector <QLabel*> m_StatLabel;
		std::vector <QPen> m_CaseStyle;
		std::vector <QPen> m_AtlasStyle;
		std::vector <QPen> m_StatStyle;
		QLCDNumber* m_CaseLcd;
		QSlider* m_CaseSlider;
		QLabel* m_CasePxSize;
		QLCDNumber* m_AtlasLcd;
		QSlider* m_AtlasSlider;
		QLabel* m_AtlasPxSize;
		QLCDNumber* m_StatLcd;
		QSlider* m_StatSlider;
		QLabel* m_StatPxSize;
		QPushButton* m_ComputeCorr;
		QPushButton* m_DecomputeCorr;
		QSlider* m_ThSlider;
		QLCDNumber* m_ThLcd;
		QLabel* m_ThLabel;
		QTextEdit* m_CorrText;
		QwtPlot* m_Plot;
		std::vector <std::vector <std::vector <QwtPlotCurve*> > > m_CaseCurves;
		std::vector <std::vector <std::vector <QwtPlotCurve*> > > m_AtlasCurves;
		std::vector <std::vector <std::vector <QwtPlotCurve*> > > m_StatCurves;
		
		
		DTIAtlasFiberAnalyzerguiwindow* m_parent;
		int m_SpecialCurves;
		vstring m_Parameters;
		vstring m_Cases;
		vstring m_Fibers;
		qv3double m_Corr;
		bool m_Error;
		
};

#endif
