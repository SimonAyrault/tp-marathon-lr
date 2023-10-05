#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Initialisation de l'interface graphique
    ui->setupUi(this);

    // Instanciation de la socket
    tcpSocket = new QTcpSocket(this);

    // Attachement d'un slot qui sera appelé à chaque fois que des données arrivent (mode asynchrone)
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(gerer_donnees()));

    // Idem pour les erreurs
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(afficher_erreur(QAbstractSocket::SocketError)));

    // Instanciation du timer
    pTimer = new QTimer();

    // Association du "tick" du timer à l'appel d'une méthode SLOT
    connect(pTimer, SIGNAL(timeout()), this, SLOT(mettre_a_jour_ihm()));

    // Lancement du timer avec un tick toutes les 1000 ms
    pTimer->start(1000);

    // Instanciation de l'image
    pCarte = new QImage();
    pCarte->load(":/carte_la_rochelle_plan.png");

    // "Connexion" à la base de données SQLite
    bdd = QSqlDatabase::addDatabase("QSQLITE");
    bdd.setDatabaseName(":/marathon.sqlite");
    if (!bdd.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }

}

MainWindow::~MainWindow()
{
    // Destruction de la socket
    tcpSocket->abort();
    delete tcpSocket;

    // Arrêt du timer
    pTimer->stop();
    //Destruction du timer
    delete pTimer;

    // Suppression de l'image
    delete pCarte;

    // Destruction de l'interface graphique
    delete ui;

    // Fermeture de la base de donnée
    bdd.close();

}

void MainWindow::on_connexionButton_clicked()
{
    // Récupération des paramètres
    QString adresse_ip = ui->lineEdit_ip->text();
    unsigned short port_tcp = ui->lineEdit_port->text().toInt();

    // Connexion au serveur
    tcpSocket->connectToHost(adresse_ip, port_tcp);
}

void MainWindow::on_deconnexionButton_clicked()
{
    // Déconnexion du serveur
    tcpSocket->close();
}

void MainWindow::on_envoiButton_clicked()
{
    // Préparation de la requête
    QByteArray requete;
    requete = "RETR\r\n";

    // Envoi de la requête
    tcpSocket->write(requete);
}
void MainWindow::gerer_donnees()

{
    // Réception des données
    QByteArray reponse = tcpSocket->readAll();
    QString trame = QString(reponse);
    QStringList list = trame.split(",");


    // Affichage
    ui->lineEdit_reponse->setText(QString(reponse));

    //Date
    int heures = list[1].mid(0,2).toInt();
    int minutes = list[1].mid(2,2).toInt();
    int secondes = list[1].mid(4,2).toInt();
    int timestamp = (heures * 3600 + minutes *60 + secondes);
    qDebug() << "timestamp :" << timestamp;
    QString timestampQString = QString("%1").arg(timestamp);
    ui->lineEdit_Heure->setText(timestampQString);

    // Latitude
    double Latitude_deg = list[2].mid(0,2).toDouble();
    double Latitude_min = list[2].mid(2,7).toDouble();
    double Latitude = 0.0;
    if( list[3] == "S"){
        Latitude = (Latitude_deg + (Latitude_min / 60))*(-1);
        qDebug() << "Latitude :" << Latitude;
        QString LatitudeQString = QString("%1").arg(Latitude);
        ui->lineEdit_Latitude->setText(LatitudeQString);
    }else if(list[3] == "N"){
        Latitude = (Latitude_deg + (Latitude_min / 60));
        qDebug() << "Latitude :" << Latitude;
        QString LatitudeQString = QString("%1").arg(Latitude);
        ui->lineEdit_Latitude->setText(LatitudeQString);

    }else{
        Latitude = (Latitude_deg + (Latitude_min / 60));
        qDebug() << "Latitude :" << Latitude;
        QString LatitudeQString = QString("%1").arg(Latitude);
        ui->lineEdit_Latitude->setText(LatitudeQString);
    }


    //Longitude
    double Longitude_deg =list[4].mid(0,3).toDouble();
    double Longitude_min = list[4].mid(3,7).toDouble();
    double Longitude = 0.0;

    if (list[5] == "W"){
        Longitude = (Longitude_deg + (Longitude_min / 60))*(-1);
        qDebug() << "Longitude :" << Longitude;
        QString LongitudeQString = QString("%1").arg(Longitude);
        ui->lineEdit_Longitude->setText(LongitudeQString);
    }

    else {
        Longitude = (Longitude_deg + (Longitude_min / 60));
        qDebug() << "Longitude :" << Longitude;
        QString LongitudeQString = QString("%1").arg(Longitude);
        ui->lineEdit_Longitude->setText(LongitudeQString);
    }

    //Altitude
    QString Altitude = list[9];
    ui->lineEdit_Altitude->setText(Altitude);

    // Préparation du contexte de dessin sur une image existante
    QPainter p(pCarte);

    // Choix de la couleur
    double long_hg = -1.195703 ;
    double long_bd =  -1.136125;
    double lat_hg = 46.173311;
    double lat_bd = 46.135451;
    double hauteur = 638.0;
    double largeur = 694.0;
    // Dessin d'une ligne
    px = largeur * ( (Longitude - long_hg ) / (long_bd - long_hg) );
    py = hauteur * ( 1.0 - (Latitude - lat_bd) / (lat_hg - lat_bd) );
    if  (px0 !=0.0 and py0 != 0.0)
        {
            p.setPen(Qt::red);
            p.drawLine(px0, py0, px, py);
            // Fin du dessin et application sur l'image
            p.end();
            ui->label_carte->setPixmap(QPixmap::fromImage(*pCarte));
    }else{}

    //Fréquence cardiaque
    QString frequence = list[14];
    double freq = frequence.mid(1,3).toInt();
    QString freq_string = QString("%1").arg(freq);
    ui->lineEdit_FC->setText(freq_string);

    //FC Max
    double age = ui->spinBox_age->value();
    double FCMax = 207 -(0.7 * age);
    QString FCMaxQString = QString("%1").arg(FCMax);
    ui->lineEdit_FCMax->setText(FCMaxQString);

    // Calcul de l'intensité de l'effort
    double intensité = (freq / FCMax * 100);
    qDebug() << "Intensité" << intensité;
    ui->progressBar->setValue(intensité);

}

void MainWindow::mettre_a_jour_ihm()
{
    // Préparation de la requête
    QByteArray requete;
    requete = "RETR\r\n";

    // Envoi de la requête
    tcpSocket->write(requete);
    qDebug() << "tic";

    px0 = px;
    py0 = py;
}

void MainWindow::afficher_erreur(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, tr("Client TCP"),
                                     tr("Hôte introuvable"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, tr("Client TCP"),
                                     tr("Connexion refusée"));
            break;
        default:
            QMessageBox::information(this, tr("Client TCP"),
                                     tr("Erreur : %1.")
                                     .arg(tcpSocket->errorString()));
    }
}
