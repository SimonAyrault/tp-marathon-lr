#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <bitset>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QCoreApplication>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connexionButton_clicked();

    void on_deconnexionButton_clicked();

    void on_envoiButton_clicked();

    void gerer_donnees();

    void afficher_erreur(QAbstractSocket::SocketError);

    void mettre_a_jour_ihm();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    QTimer *pTimer;
    QImage *pCarte;
    QImage *pCarte2;
    QImage *pInvisible;
    QSqlDatabase bdd;
    // Calcul
    double px ;
    double py ;
    double px0 ;
    double py0 ;
    double LatA ;
    double LatB ;
    double LonA ;
    double LonB ;
    int timestamp ;
    int last_timestamp ;
    double distance ;
    double calcul_distance ;
    double lastdistance ;

};

#endif // MAINWINDOW_H
