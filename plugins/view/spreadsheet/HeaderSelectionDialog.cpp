#include "HeaderSelectionDialog.h"
#include "ui_HeaderSelectionDialog.h"
#include <QtCore/QRegExp>

using namespace std;

HeaderSelectionDialog::HeaderSelectionDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::HeaderSelectionDialog)
{
    ui->setupUi(this);
    connect(ui->displayAllColumnsPushButton,SIGNAL(clicked(bool)),this,SLOT(showAll()));
    //    connect(ui->hideAllColumnsPushButton,SIGNAL(clicked(bool)),this,SLOT(hideAll()));
    connect(ui->displayUserPushButton,SIGNAL(clicked(bool)),this,SLOT(showUser()));
    connect(ui->displayVisualPushButton,SIGNAL(clicked(bool)),this,SLOT(showVisual()));
}

HeaderSelectionDialog::~HeaderSelectionDialog()
{
    delete ui;
}

void HeaderSelectionDialog::setHeaderView(QHeaderView* headerView){
    ui->listWidget->clear();
    for(int i=0;i<headerView->count();++i){
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        item->setText(headerView->model()->headerData(i,headerView->orientation()).toString());
        item->setData(Qt::UserRole,QVariant(i));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(headerView->isSectionHidden(i)?Qt::Unchecked:Qt::Checked);
        ui->listWidget->addItem(item);
    }
}

set<int> HeaderSelectionDialog::selectedIndexes()const{
    set<int> selected;
    for(int i = 0 ; i< ui->listWidget->count();++i){
        QListWidgetItem* item = ui->listWidget->item(i);
        if(item->checkState() == Qt::Checked){
            selected.insert(item->data(Qt::UserRole).toInt());
        }
    }
    return selected;
}

void HeaderSelectionDialog::updateHeaders(QHeaderView* header,QWidget* parent){
    HeaderSelectionDialog *dialog = new HeaderSelectionDialog(parent);
    dialog->setWindowTitle(tr("Choose columns to display"));
    dialog->setHeaderView(header);
    if(dialog->exec() == QDialog::Accepted){
        set<int> selection = dialog->selectedIndexes();
        for(int i=0;i<header->count();++i){
            header->setSectionHidden(i,selection.find(i)==selection.end());
        }
    }
}

void HeaderSelectionDialog::hideAll(){
    for(int i = 0 ; i <ui->listWidget->count() ; ++i){
        ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
    }
}

void HeaderSelectionDialog::showAll(){
    for(int i = 0 ; i <ui->listWidget->count() ; ++i){
        ui->listWidget->item(i)->setCheckState(Qt::Checked);
    }
}

void HeaderSelectionDialog::showVisual(){
    QRegExp regexp("^view|^\\*view");
    for(int i = 0 ; i <ui->listWidget->count() ; ++i){
        //Check if it's a visual properties
        if(regexp.indexIn(ui->listWidget->item(i)->text())!=-1){
            ui->listWidget->item(i)->setCheckState(Qt::Checked);
        }else{
            ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
        }
    }
}

void HeaderSelectionDialog::showUser(){
    QRegExp regexp("^view|^\\*view");
    for(int i = 0 ; i <ui->listWidget->count() ; ++i){
        if(regexp.indexIn(ui->listWidget->item(i)->text()) == -1){
            ui->listWidget->item(i)->setCheckState(Qt::Checked);
        }else{
            ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
        }
    }
}
