#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../lib/fptree_headeronly.h"

#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QStandardItemModel(this);
    ui->treeView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                          QDir::currentPath(),
                                                          tr("Database (*.csv)"));
    ui->lineEdit->setText(fileName);

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(nullptr, "error", file.errorString());
        return;
    }

    // Remove actual data
    ui->textEdit->clear();

    QTextStream in(&file);

    ItemSupport<QString> itemsupport(0);
    using Transactions = vector<Transaction<QString>>;

    // Define transactions
    Transactions tr;
    while(!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(";");

        vector<QString> transaction;
        for(QString item:fields)
        {
            transaction.push_back(item);
            itemsupport << item;
            cout << item.toStdString() << " ";
        }

        tr.push_back(transaction);
        ui->textEdit->append(line);
    }
    file.close();

    // Construct FP-Tree
    QString root("*");
    FP_Tree<QString> fptree(itemsupport, root);
    for(Transaction<QString>& item : tr)
    {
        fptree.construct(item);
    }

    // Add nodes to the view
    model->clear();
    QStandardItem* item = new QStandardItem(fptree.root()->_itemValue);

    function<void(Node<QString> *, QStandardItem*, int)> fctPrint;
    fctPrint = [&fctPrint](Node<QString> *p_actualNode, QStandardItem* p_item, int p_level)
    {
        string tab;
        for(int l=0; l<p_level; ++l)
        {
            tab += "  ";
        }

        p_item->setData(p_actualNode->_itemValue);
        cout << tab << '-' << p_actualNode->_itemValue.toStdString() << " (freq " << p_actualNode->_freq << ")" << endl;
        if(p_actualNode && !p_actualNode->_children.empty())
        {
            ++p_level;
            int nChild = 0;
            for(Node<QString>* node : p_actualNode->_children)
            {
                QStandardItem* itemChild = new QStandardItem(node->_itemValue
                                                             + "(" + QString::number(node->_freq) + ")");
                p_item->setChild(nChild++, itemChild);

                fctPrint(node, itemChild, p_level);
            }
        }
    };

    fctPrint(fptree.root(), item, 0);

    QList<QStandardItem*> listeItems{item};
    model->appendRow(listeItems);
    ui->treeView->expandAll();
}
