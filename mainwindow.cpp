#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	m_nPointsNum(0)
{
    ui->setupUi(this);

	Init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open point cloud file"), QString::fromLocal8Bit(m_cloud.strDirName.c_str()), tr("Point cloud data(*.pcd *.ply *.obj);;All file(*.*)"));
	if (fileNames.empty())
	{
		return;
	}

	for (int i = 0; i < fileNames.size(); ++i)
	{
		TimeStart();

		m_cloud.ptrCloud.reset(new Cloud);

		QString strPathName = fileNames[i];
		std::string strFileName = strPathName.toStdString();
		std::string strSubName = GetFileName(strFileName);

		int nStatus = -1;
		if (strPathName.endsWith(".pcd", Qt::CaseInsensitive))
		{
			nStatus = pcl::io::loadPCDFile(strFileName, *(m_cloud.ptrCloud));
			if (m_cloud.ptrCloud->points[0].r == 0 && m_cloud.ptrCloud->points[0].g == 0 && m_cloud.ptrCloud->points[0].b == 0)
			{
				SetCloudColor(255, 255, 255);
			}
		}
		else if (strPathName.endsWith(".ply", Qt::CaseInsensitive))
		{
			nStatus = pcl::io::loadPLYFile(strFileName, *(m_cloud.ptrCloud));
			if (m_cloud.ptrCloud->points[0].r == 0 && m_cloud.ptrCloud->points[0].g == 0 && m_cloud.ptrCloud->points[0].b == 0)
			{
				SetCloudColor(255, 255, 255);
			}
		}
		else if (strPathName.endsWith(".obj", Qt::CaseInsensitive))
		{
			nStatus = pcl::io::loadOBJFile(strFileName, *(m_cloud.ptrCloud));
			if (m_cloud.ptrCloud->points[0].r == 0 && m_cloud.ptrCloud->points[0].g == 0 && m_cloud.ptrCloud->points[0].b == 0)
			{
				SetCloudColor(255, 255, 255);
			}
		}
		else
		{
			//提示：无法读取除了.ply .pcd .obj以外的文件
			QMessageBox::information(this, tr("File format error"), tr("Can't open files except .ply .pcd .obj"));
			return;
		}

		if (nStatus != 0)
		{
			QMessageBox::critical(this, tr("Reading file error"), tr("We can not open the file"));
			return;
		}

		SetA(255);

		m_cloud.strPathName = strFileName;
		m_cloud.strFileName = strSubName;
		m_cloud.strDirName = strFileName.substr(0, strFileName.size() - strSubName.size());
		m_vctCloud.push_back(m_cloud);  //将点云导入点云容器

		QString timeDiff = TimeOff();

		// 设置资源管理器
		QTreeWidgetItem *cloudName = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit(strSubName.c_str()));
		cloudName->setIcon(0, QIcon(":/resource/images/icon.png"));
		ui->dataTree->addTopLevelItem(cloudName);

		// 输出窗口
		ConsoleLog("Add", QString::fromLocal8Bit(m_cloud.strFileName.c_str()), QString::fromLocal8Bit(m_cloud.strPathName.c_str()), "Time cost: " + timeDiff + " s, Points: " + QString::number(m_cloud.ptrCloud->points.size()));

		m_nPointsNum += m_cloud.ptrCloud->points.size();
	}

	ViewerAddedCloud();
	SetPropertyTable();
}

void MainWindow::on_actionClear_triggered()
{
	m_vctCloud.clear();

	viewer->removeAllPointClouds();
	viewer->removeAllShapes();

	ui->dataTree->clear();
	ui->propertyTable->clear();   //清空属性窗口propertyTable

	QStringList header;
	header << "Property" << "Value";
	ui->propertyTable->setHorizontalHeaderLabels(header);

	//输出窗口
	ConsoleLog("Clear", "All point clouds", "", "");
	//更新显示
	ViewerCloud(); 
}

void MainWindow::on_actionSave_triggered()
{
	QString strSaveName = QFileDialog::getSaveFileName(this, tr("Save point cloud"),
		QString::fromLocal8Bit(m_cloud.strDirName.c_str()), tr("Point cloud data(*.pcd *.ply);;Allfile(*.*)"));
	std::string strFileName = strSaveName.toStdString();
	std::string strSubName = GetFileName(strFileName);

	if (strSaveName.isEmpty())
	{
		return;
	}

	if (m_vctCloud.size() > 1)
	{
		SaveMultiCloud();
		return;
	}

	int nStatus = -1;
	if (strSaveName.endsWith(".pcd", Qt::CaseInsensitive))
	{
		nStatus = pcl::io::savePCDFile(strFileName, *(m_cloud.ptrCloud));
	}
	else if (strSaveName.endsWith(".ply", Qt::CaseInsensitive))
	{
		nStatus = pcl::io::savePLYFile(strFileName, *(m_cloud.ptrCloud));
	}
	
	else //提示：无法保存为除了.ply .pcd以外的文件
	{
		QMessageBox::information(this, tr("File format error"),
			tr("Can't save files except .ply .pcd"));
		return;
	}
	//提示：后缀没问题，但是无法保存
	if (nStatus != 0)
	{
		QMessageBox::critical(this, tr("Saving file error"),
			tr("We can not save the file"));
		return;
	}

	//输出窗口
	ConsoleLog("Save", QString::fromLocal8Bit(strSubName.c_str()), strSaveName, "Single save");

	setWindowTitle(strSaveName + " - EasyCloud");

	QMessageBox::information(this, tr("save point cloud file"),
		QString::fromLocal8Bit(("Save " + strSubName + " successfully!").c_str()));
}

void MainWindow::on_actionExit_triggered()
{
	this->close();
}

void MainWindow::on_actionUp_triggered()
{
	if (!m_pCloud->empty())
	{
		viewer->setCameraPosition(0.5*(m_PointMin.x + m_PointMax.x), 0.5*(m_PointMin.y + m_PointMax.y), m_PointMax.z + 5 * m_dMaxLen, 0.5*(m_PointMin.x + m_PointMax.x), 0.5*(m_PointMin.y + m_PointMax.y), m_PointMax.z, 0, 1, 0);
		ui->qvtkWidget->update();
	}
}

void MainWindow::on_actionBottom_triggered()
{
	if (!m_pCloud->empty())
	{
		viewer->setCameraPosition(0.5*(m_PointMin.x + m_PointMax.x), 0.5*(m_PointMin.y + m_PointMax.y), m_PointMin.z - 5 * m_dMaxLen, 0.5*(m_PointMin.x + m_PointMax.x), 0.5*(m_PointMin.y + m_PointMax.y), m_PointMax.z, 0, 1, 0);
		ui->qvtkWidget->update();
	}
}

void MainWindow::on_actionFront_triggered()
{
	if (!m_pCloud->empty())
	{
		viewer->setCameraPosition(0.5*(m_PointMin.x + m_PointMax.x), m_PointMin.y - 5 * m_dMaxLen, 0.5*(m_PointMin.z + m_PointMax.z), 0.5*(m_PointMin.x + m_PointMax.x), m_PointMin.y, 0.5*(m_PointMin.z + m_PointMax.z), 0, 0, 1);
		ui->qvtkWidget->update();
	}
}

void MainWindow::on_actionBack_triggered()
{
	if (!m_pCloud->empty())
	{
		viewer->setCameraPosition(0.5*(m_PointMin.x + m_PointMax.x), m_PointMax.y + 5 * m_dMaxLen, 0.5*(m_PointMin.z + m_PointMax.z), 0.5*(m_PointMin.x + m_PointMax.x), m_PointMin.y, 0.5*(m_PointMin.z + m_PointMax.z), 0, 0, 1);
		ui->qvtkWidget->update();
	}
}

void MainWindow::on_actionLeft_triggered()
{
	if (!m_pCloud->empty())
	{
		viewer->setCameraPosition(m_PointMin.x - 5 * m_dMaxLen, 0.5*(m_PointMin.y + m_PointMax.y), 0.5*(m_PointMin.z + m_PointMax.z), m_PointMax.x, 0.5*(m_PointMin.y + m_PointMax.y), 0.5*(m_PointMin.z + m_PointMax.z), 0, 0, 1);
		ui->qvtkWidget->update();
	}
}

void MainWindow::on_actionRight_triggered()
{
	if (!m_pCloud->empty())
	{
		viewer->setCameraPosition(m_PointMax.x + 5 * m_dMaxLen, 0.5*(m_PointMin.y + m_PointMax.y), 0.5*(m_PointMin.z + m_PointMax.z), m_PointMax.x, 0.5*(m_PointMin.y + m_PointMax.y), 0.5*(m_PointMin.z + m_PointMax.z), 0, 0, 1);
		ui->qvtkWidget->update();
	}
}

void MainWindow::Init()
{
	// 界面初始化
	setWindowIcon(QIcon(tr(":/resource/images/icon.png")));
	setWindowTitle(tr("EasyCloud"));

	//点云初始化
	m_cloud.ptrCloud.reset(new Cloud);
	m_cloud.ptrCloud->resize(1);

	m_pCloud.reset(new Cloud);
	m_pInputCloud.reset(new Cloud);

	//可视化对象初始化
	viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	//设置VTK可视化窗口指针
	ui->qvtkWidget->SetRenderWindow(viewer->getRenderWindow());
	//设置窗口交互，窗口可接受键盘等事件
	viewer->setupInteractor(ui->qvtkWidget->GetInteractor(), ui->qvtkWidget->GetRenderWindow());
	ui->qvtkWidget->update();

	ui->propertyTable->setSelectionMode(QAbstractItemView::NoSelection);	// 禁止点击属性管理器的 item
	ui->consoleTable->setSelectionMode(QAbstractItemView::NoSelection);		// 禁止点击输出窗口的 item
	ui->dataTree->setSelectionMode(QAbstractItemView::ExtendedSelection);	// 允许dataTree进行多选

	//设置默认主题
	/*QString qss = darcula_qss;
	qApp->setStyleSheet(qss);*/
	//属性管理窗口
	SetPropertyTable();
	// 输出窗口
	SetConsoleTable();

	ConsoleLog("Software start", "EasyCloud", "Welcome to use EasyCloud", "Z");
	// 设置背景颜色为 dark
	viewer->setBackgroundColor(30 / 255.0, 30 / 255.0, 30 / 255.0);
}

void MainWindow::SetPropertyTable()
{
	QStringList header;
	header << "Property" << "Value";

	ui->propertyTable->setHorizontalHeaderLabels(header);
	ui->propertyTable->setItem(0, 0, new QTableWidgetItem("Clouds"));
	ui->propertyTable->setItem(1, 0, new QTableWidgetItem("Points"));
	ui->propertyTable->setItem(2, 0, new QTableWidgetItem("Total points"));
	ui->propertyTable->setItem(3, 0, new QTableWidgetItem("RGB"));

	ui->propertyTable->setItem(0, 1, new QTableWidgetItem(QString::number(m_vctCloud.size())));
	ui->propertyTable->setItem(1, 1, new QTableWidgetItem(""));
	ui->propertyTable->setItem(2, 1, new QTableWidgetItem(QString::number(m_nPointsNum)));
	ui->propertyTable->setItem(4, 1, new QTableWidgetItem(""));
}

void MainWindow::SetConsoleTable() {
	// 设置输出窗口
	QStringList header;
	header << "Time" << "Operation" << "Operation object" << "Details" << "Note";

	ui->consoleTable->setHorizontalHeaderLabels(header);
	ui->consoleTable->setColumnWidth(0, 150);
	ui->consoleTable->setColumnWidth(1, 200);
	ui->consoleTable->setColumnWidth(2, 200);
	ui->consoleTable->setColumnWidth(3, 300);

	//ui.consoleTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
	ui->consoleTable->verticalHeader()->setDefaultSectionSize(22);		   //设置行距

	ui->consoleTable->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::ConsoleLog(QString operation, QString subName, QString fileName, QString note)
{
	if (!m_bEnableConsole)
	{
		return;
	}

	int nRowsNum = ui->consoleTable->rowCount();
	ui->consoleTable->setRowCount(++nRowsNum);
	QDateTime time = QDateTime::currentDateTime();			//获取系统现在的时间
	QString strTime = time.toString("MM-dd hh:mm:ss");		//设置显示格式
	ui->consoleTable->setItem(nRowsNum - 1, 0, new QTableWidgetItem(strTime));
	ui->consoleTable->setItem(nRowsNum - 1, 1, new QTableWidgetItem(operation));
	ui->consoleTable->setItem(nRowsNum - 1, 2, new QTableWidgetItem(subName));
	ui->consoleTable->setItem(nRowsNum - 1, 3, new QTableWidgetItem(fileName));
	ui->consoleTable->setItem(nRowsNum - 1, 4, new QTableWidgetItem(note));

	ui->consoleTable->scrollToBottom();						// 滑动自动滚到最底部
}

void MainWindow::ViewerCloud()
{
	for (int i = 0; i != m_vctCloud.size(); i++)
	{
		viewer->updatePointCloud(m_vctCloud[i].ptrCloud, "cloud" + QString::number(i).toStdString());
	}
	//viewer->resetCamera();
	ui->qvtkWidget->update();
}

void MainWindow::ViewerAddedCloud()
{
	for (int i = 0; i != m_vctCloud.size(); i++)
	{
		// 添加到窗口
		viewer->addPointCloud(m_vctCloud[i].ptrCloud, "cloud" + QString::number(i).toStdString());
		viewer->updatePointCloud(m_vctCloud[i].ptrCloud, "cloud" + QString::number(i).toStdString());
	}

	//重设视角
	viewer->resetCamera();

	//刷新窗口
	ui->qvtkWidget->update();
}

void MainWindow::UpdateDisplay()
{
	// 清空点云
	m_pCloud->clear();
	viewer->removeAllPointClouds();
	viewer->removeAllCoordinateSystems();

	// 点云中是否包含无效值
	if (m_pInputCloud->is_dense)
	{
		pcl::copyPointCloud(*m_pInputCloud, *m_pCloud);
	}
	else
	{
		PCL_WARN("Cloud is not dense! Non finite points will be removed\n");
		std::vector<int> vec;
		pcl::removeNaNFromPointCloud(*m_pInputCloud, *m_pCloud, vec);
	}

	// 添加到窗口
	viewer->addPointCloud(m_pCloud);

	pcl::getMinMax3D(*m_pCloud, m_PointMin, m_PointMax);

	m_dMaxLen = GetMaxValue(m_PointMin, m_PointMax);

	//重设视角
	viewer->resetCamera();

	//刷新窗口
	ui->qvtkWidget->update();
}

double MainWindow::GetMaxValue(PointT p1, PointT p2)
{
	double max = 0;

	if (p1.x - p2.x > p1.y - p2.y)
	{
		max = p1.x - p2.x;

	}
	else
	{
		max = p1.y - p2.y;
	}

	if (max < p1.z - p2.z)
	{
		max = p1.z - p2.z;
	}

	return max;
}

void MainWindow::TimeStart()
{
	time.start();
}

QString MainWindow::TimeOff()
{
	int nTimeDiff = time.elapsed();		//返回从上次start()或restart()开始以来的时间差，单位ms
	QString strTimediff = QString("%1").arg(nTimeDiff /1000.0);  //float->QString
	return strTimediff;
}

std::string MainWindow::GetFileName(std::string strFileName)
{
	std::string strSubName;

	for (auto i = strFileName.end()- 1; *i!='/'; i--)
	{
		strSubName.insert(strSubName.begin(), *i);
	}

	return strSubName;
}

void MainWindow::SetCloudColor(unsigned int r, unsigned int g, unsigned int b)
{
	for (size_t i = 0; i < m_cloud.ptrCloud->size(); i++)
	{
		m_cloud.ptrCloud->points[i].r = r;
		m_cloud.ptrCloud->points[i].g = g;
		m_cloud.ptrCloud->points[i].b = b;
		m_cloud.ptrCloud->points[i].a = 255;
	}
}

void MainWindow::SetA(unsigned int a)
{
	for (size_t i = 0; i < m_cloud.ptrCloud->size(); i++)
	{
		m_cloud.ptrCloud->points[i].a = a;
	}
}

void MainWindow::SaveMultiCloud()
{

}