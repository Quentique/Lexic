#include <poppler-qt5.h>
#include <QAction>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPrinterInfo>
#include <QPrintPreviewDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QSignalMapper>
#include <QtXml>

#include "style.h"
#include "option.h"
#include "aide.h"
#include "propos.h"
#include "fenetre.h"
#include "web.h"
#include "ajout.h"
#include "modifier.h"

Fenetre::Fenetre()
{

   zoneprincipale = new QWidget;
   ajout = new QPushButton;
   modifier = new QPushButton;
   afficher = new QPushButton;
   supprimer = new QPushButton;
   arbre = new QTreeWidget;

   QVBoxLayout *layoutv = new QVBoxLayout;
   QHBoxLayout *layouth = new QHBoxLayout;

   ajout->setIcon(QIcon(":/images/ajouter.png"));
   ajout->setIconSize(QPixmap(":/images/modifier.png").size());
   ajout->setMinimumSize(50, 50);
   ajout->setMaximumSize(50, 50);

   modifier->setIcon(QIcon(":/images/modifier.png"));
   modifier->setIconSize(QSize(42, 42));
   modifier->setMinimumSize(50, 50);
   modifier->setMaximumSize(50, 50);

   afficher->setIcon(QIcon(":/images/afficher.png"));
   afficher->setIconSize(QPixmap(":images/modifier.png").size());
   afficher->setMinimumSize(50, 50);
   afficher->setMaximumSize(50, 50);

   supprimer->setIcon(QIcon(":/images/supprimer.png"));
   supprimer->setIconSize(QPixmap(":images/supprimer.png").size());
   supprimer->setMinimumSize(50, 50);
   supprimer->setMaximumSize(50, 50);

   modifier->setEnabled(false);
   afficher->setEnabled(false);
   supprimer->setEnabled(false);

   layoutv->addWidget(ajout);
   layoutv->addWidget(modifier);
   layoutv->addWidget(afficher);
   layoutv->addWidget(supprimer);

   layouth->addWidget(arbre);
   layouth->addLayout(layoutv);

   QMenu *menufichier = menuBar()->addMenu(tr("&Fichier"));
   QMenu *menuoption = menuBar()->addMenu(tr("&Options"));
   QMenu *menuaide = menuBar()->addMenu(tr("&Aide"));

   QAction *quitter = new QAction(tr("&Quitter"), this);
   QAction *parametre = new QAction(tr("&Options"), this);
   QAction *stylegestion = new QAction(tr("Gérer le style"), this);
   QAction *aide = new QAction(tr("&Aide"), this);
   QAction *about = new QAction(tr("A propos..."), this);
   QAction *majcheck = new QAction(tr("&Vérifier les mises à jour"), this);
   exportation = new QAction(tr("Exporter"), this);
   impression = new QAction(tr("Imprimer"), this);
   apercuavant = new QAction(tr("Aperçu avant impression"), this);

   quitter->setShortcut(QKeySequence(tr("Ctrl+Q")));
   exportation->setShortcut(QKeySequence(tr("Ctrl+E")));
   impression->setShortcut(QKeySequence(tr("Ctrl+P")));

   parametre->setShortcut(QKeySequence(tr("Ctrl+O")));
   stylegestion->setShortcut(QKeySequence(tr("Ctrl+S")));
   aide->setShortcut(QKeySequence(tr("F1")));

   exportation->setEnabled(false);
   impression->setEnabled(false);
   apercuavant->setEnabled(false);

   menufichier->addAction(quitter);
   menufichier->addAction(exportation);
   menufichier->addAction(impression);
   menufichier->addAction(apercuavant);

   menuoption->addAction(stylegestion);
   menuoption->addAction(parametre);
   menuoption->addAction(majcheck);

   menuaide->addAction(aide);
   menuaide->addAction(about);

   zoneprincipale->setLayout(layouth);

   setCentralWidget(zoneprincipale);
   setWindowTitle("WordBook");
   setFixedSize(435, 280);

   QStringList liste;
   liste << tr("Nom") << tr("Langue");

   arbre->setHeaderLabels(liste);
   arbre->setColumnWidth(0, 200);
   arbre->setColumnWidth(1, 150);

   lister();

   QObject::connect(arbre, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(affiche_page(QTreeWidgetItem*,int)));
   QObject::connect(afficher, SIGNAL(clicked()), this, SLOT(affiche()));
   QObject::connect(supprimer, SIGNAL(clicked()), this, SLOT(supprime()));
   QObject::connect(ajout, SIGNAL(clicked()), this, SLOT(ajouter()));
   QObject::connect(modifier, SIGNAL(clicked()), this, SLOT(changer()));
   QObject::connect(arbre, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(degriser()));
   QObject::connect(quitter, SIGNAL(triggered()), qApp, SLOT(quit()));
   QObject::connect(stylegestion, SIGNAL(triggered()), this, SLOT(css()));
   QObject::connect(parametre, SIGNAL(triggered()), this, SLOT(options()));
   QObject::connect(aide, SIGNAL(triggered()), this, SLOT(aide_aff()));
   QObject::connect(about, SIGNAL(triggered()), this, SLOT(apropos()));
   QObject::connect(exportation, SIGNAL(triggered()), this, SLOT(pdf()));
   QObject::connect(impression, SIGNAL(triggered()), this, SLOT(clicPrint()));
   QObject::connect(apercuavant, SIGNAL(triggered()), this, SLOT(apercu()));

   QSignalMapper *mapper = new QSignalMapper;
   mapper->setMapping(majcheck, "false");

   QObject::connect(majcheck, SIGNAL(triggered()), mapper, SLOT(map()));
   QObject::connect(mapper, SIGNAL(mapped(QString)), this, SLOT(maj(QString)));
}
void Fenetre::affiche_page(QTreeWidgetItem* slot, int te)
{
   Web testg(slot->text(0));
}
void Fenetre::apropos()
{
   Propos *apropos_aff = new Propos;
   apropos_aff->show();
}
void Fenetre::print(QPrinter *imprimante)
{
   QProcess *process = new QProcess;
   QStringList arguments;
   arguments << QCoreApplication::applicationDirPath() + "/data/" + arbre->selectedItems().at(0)->text(0) + ".html " << QCoreApplication::applicationDirPath() + "/data/export.pdf";
   process->start("wkhtmltopdf.exe", arguments);
   process->waitForFinished();

    Poppler::Document* document = Poppler::Document::load(QCoreApplication::applicationDirPath() + "/data/export.pdf");

    if (document == 0)
    {
        QMessageBox::critical(this, "Erreur", "Le programme n'a pas pu effectuer l'action demandé\nMerci de reporter le bug : Error during document opening");
    }

    document->setRenderBackend(Poppler::Document::SplashBackend);
    document->setRenderHint(Poppler::Document::Antialiasing, true);
    document->setRenderHint(Poppler::Document::TextAntialiasing, true);

    if (document->isLocked())
    {
        delete document;
    }

    int printerResolution = imprimante->resolution();

    int paperWitdh = imprimante->paperRect(QPrinter::DevicePixel).width();
    int paperHeight = imprimante->paperRect(QPrinter::DevicePixel).height();

    int pageWidth = imprimante->pageRect(QPrinter::DevicePixel).width();
    int pageHeight = imprimante->pageRect(QPrinter::DevicePixel).height();

    int numberOfPages = document->numPages();

    QPainter painter;
    painter.begin(imprimante);

    for (int currentPageNumber = 0; currentPageNumber < numberOfPages; currentPageNumber++)
    {
        if (currentPageNumber != 0)
        {
            imprimante->newPage();
        }

        Poppler::Page* pdfPage = document->page(currentPageNumber);

        if (pdfPage == 0)
        {
            QMessageBox::critical(this, "Erreur", "Le programme n'a pas pu effectuer l'action demandée\nMerci de reporter le bug : Error during page opening");
        }

        QImage printImage = pdfPage->renderToImage(printerResolution, printerResolution, 0, 0, paperWitdh, paperHeight);
        painter.drawPixmap(0, 0, pageWidth, pageHeight, QPixmap::fromImage(printImage));
    }

    painter.end();
    QFile::remove(QCoreApplication::applicationDirPath() + "/data/export.pdf");
}
void Fenetre::clicPrint()
{
    QPrinter printer;
    QPrintDialog dialog(&printer);

    dialog.exec();
    print(&printer);
}
void Fenetre::apercu()
{
    QPrinter printer2;
    QPrintPreviewDialog dialog2(&printer2, this);

    QObject::connect(&dialog2, SIGNAL(paintRequested(QPrinter*)), this, SLOT(print(QPrinter*)));

    dialog2.resize(900, 900);
    dialog2.setSizeGripEnabled(true);
    dialog2.exec();
}

void Fenetre::aide_aff()
{
    Aide *aideaff = new Aide;
    aideaff->show();
}

void Fenetre::options()
{
    Option *fen_options = new Option;
    fen_options->show();
}

void Fenetre::affiche()
{
    Web testf(arbre->selectedItems().at(0)->text(0));
}
void Fenetre::supprime()
{

    QMessageBox message;
    message.setWindowTitle(tr("Confirmation"));
    message.setText(tr("Êtes-vous sûr de vouloir supprimer cette fiche : ") + arbre->selectedItems().at(0)->text(0));
    message.addButton(QMessageBox::Yes);
    message.addButton(QMessageBox::No);
    message.setButtonText(QMessageBox::Yes, tr("Oui"));
    message.setButtonText(QMessageBox::No, tr("Non"));
    int reponse = message.exec();
       if (reponse == QMessageBox::Yes)
       {
           QFile::remove(QCoreApplication::applicationDirPath() + "/data/" + arbre->selectedItems().at(0)->text(0).toLower() + ".html");
           arbre->clear();
           lister();
       }
}
void Fenetre::css()
{
    style = new Style;
    style->show();
    style->lister_parametre();
}
void Fenetre::pdf()
{
    QString fileName = arbre->selectedItems().at(0)->text(0);
    QFileDialog dialog(this);

    dialog.setWindowTitle(tr("Exportation"));
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("Fichiers PDF (*.pdf);;Joint Photographic Experts Group (*.jpeg);;Portable Network Graphics (*.png);;Bitmap (*.bmp);;Scalable Vector Graphics (*.svg)"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.selectFile(fileName + ".pdf");

    if(dialog.exec())
    {
       QFileInfo infofichier = dialog.selectedFiles().at(0);
       infofichier.suffix();
       if (infofichier.suffix() == "pdf")
       {
           export_pdf(infofichier.absoluteFilePath());
       }
       else
       {
           export_image(infofichier.absoluteFilePath());
       }

       QMessageBox *process_end = new QMessageBox;
       process_end->setStandardButtons(QMessageBox::Ok | QMessageBox::Help);
       process_end->setButtonText(QMessageBox::Ok, tr("Ok"));
       process_end->setButtonText(QMessageBox::Help, tr("Aide ?"));
       process_end->setText(tr("La commande d'exportation s'est terminée"));
       int retour = process_end->exec();

       switch(retour)
       {
          case QMessageBox::Ok:

          break;

          case QMessageBox::Help:
               QMessageBox::information(this, tr("Erreur"), tr("<h2>Votre fichier ne s'est pas créée ?</h2><ul><li>Vérifier que le fichier est enregistré dans un endroit où les permissions d'écriture sont accordés</li></ul>"));
          break;

          delete process_end;
       }
    }
}
void Fenetre::export_pdf(QString chemin)
{
    QProcess *processusexport = new QProcess(this);

    QStringList arguments;
    arguments << QCoreApplication::applicationDirPath() + "/data/" + arbre->selectedItems().at(0)->text(0) + ".html " << chemin;

    processusexport->start("wkhtmltopdf.exe", arguments);
    processusexport->waitForFinished();
}
void Fenetre::export_image(QString chemini)
{
    QProcess *processusexport = new QProcess(this);
    QStringList arguments;

    arguments << QCoreApplication::applicationDirPath() + "/data/" + arbre->selectedItems().at(0)->text(0) + ".html " << chemini;

    processusexport->start("wkhtmltoimage.exe", arguments);
    processusexport->waitForFinished();
}

void Fenetre::ajouter()
{
    fenajout = new Ajout;
    fenajout->show();

    QObject::connect(fenajout, SIGNAL(fini()), this, SLOT(rafraichir()));

}
void Fenetre::rafraichir()
{
   arbre->clear();
   lister();

   modifier->setEnabled(false);
   afficher->setEnabled(false);
   supprimer->setEnabled(false);
   exportation->setEnabled(false);
   impression->setEnabled(false);
   apercuavant->setEnabled(false);

   delete fenajout;
}
void Fenetre::changer()
{

    modif = new Modifier;
    modif->show();
    modif->affdonne(arbre->selectedItems().at(0)->text(0));

    QObject::connect(modif, SIGNAL(fini()), this, SLOT(rafraichir2()));
}
void Fenetre::rafraichir2()
{
    arbre->clear();
    lister();

    modifier->setEnabled(false);
    afficher->setEnabled(false);
    supprimer->setEnabled(false);
    exportation->setEnabled(false);
    impression->setEnabled(false);
    apercuavant->setEnabled(false);
    delete modif;
}
void Fenetre::degriser()
{
    modifier->setEnabled(true);
    afficher->setEnabled(true);
    supprimer->setEnabled(true);
    exportation->setEnabled(true);
    impression->setEnabled(true);
    apercuavant->setEnabled(true);
}
void Fenetre::maj(QString demarrage)
{
    QNetworkAccessManager manager;
    reply = manager.get(QNetworkRequest(QUrl("https://raw.githubusercontent.com/Quentique/WordBook/master/version.txt")));

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QFile fichier(QCoreApplication::applicationDirPath() + "/version.txt");
    fichier.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream flux(&fichier);
    flux.setCodec("UTF-8");
    flux << reply->readAll();

    fichier.close();

    QDomDocument *dom = new QDomDocument("mon_xml");
    if(!fichier.open(QIODevice::ReadOnly))
    {
       QMessageBox::warning(this,"Erreur à l'ouverture du document XML","Le document XML n'a pas pu être ouvert. Vérifiez que le nom est le bon et que le document est bien placé");
       return;
    }
    if (!dom->setContent(&fichier)) // Si l'on n'arrive pas à associer le fichier XML à l'objet DOM.
    {
        fichier.close();
        return;
    }
    fichier.close();
    QFile::remove(fichier.fileName());

    QDomElement doc_elements = dom->documentElement();
    doc_elements = doc_elements.firstChildElement();

    QFile version(":/texte/version.txt");
    version.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream stram(&version);
    stram.setCodec("UTF-8");
    QString vs = stram.readAll();

    if (vs < doc_elements.text())
    {
        QMessageBox newversion;
        newversion.setText(tr("Nouvelle version disponible"));

        QString versionst = doc_elements.text();
        doc_elements = doc_elements.nextSiblingElement();

        QString texte = tr("<strong>Nouvelle version : </strong>") +  versionst  + QObject::tr("<br/><strong>Version Actuelle : </strong>")+ vs + tr("<br/><u>Informations Complémentaires :</u><br/>") + doc_elements.text() + tr("<br/><strong>Souhaitez-vous la télécharger ?</strong>");
        newversion.setInformativeText(texte);
        newversion.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int retour = newversion.exec();

        if (retour == QMessageBox::Yes)
        {
            QDesktopServices::openUrl(QUrl("https://github.com/Quentique/WordBook/releases"));
            QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
        }

        version.close();
        QFile::remove(version.fileName());
    }
    else if (demarrage == "false")
    {
        QMessageBox::information(this, tr("Pas de mise à jour"), tr("Aucune nouvelle mise à jour"));
    }
}
void Fenetre::lister()
{
    QString path = QCoreApplication::applicationDirPath();
    path += "/data/";

    QDir *flux = new QDir(path);
    QStringList listes;
    listes << flux->entryList();
    int i, a;

    if (QDir(path).exists())
    {
        listes.removeFirst();
        listes.removeFirst();
    }

    i = listes.count();

    for (a = 0 ; a < i ; a++)
    {
         QTreeWidgetItem *item = new QTreeWidgetItem;
         QString affichage = listes.at(a);
         QChar lettre = affichage.at(0).toUpper();
         affichage.replace(0, 1,  lettre);
         if (affichage.endsWith(".html"))
         {
             affichage.truncate(affichage.size() - 5);
             item->setText(0, affichage);

             QFile fichierla(path + listes.at(a));
             if (!fichierla.open(QIODevice::ReadOnly))
             {
                 QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'ouvrir le fichier !"));
                 fichierla.close();
             }

             QDomDocument *dom = new QDomDocument("xml");
             if (!dom->setContent(&fichierla))
             {
                 fichierla.close();
                 QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'attribuer le fichier"));
             }

             QDomElement doc_elements = dom->documentElement();
             doc_elements = doc_elements.firstChildElement();
             doc_elements = doc_elements.nextSiblingElement();

             QDomElement tableau_donnes = doc_elements.firstChildElement();
             tableau_donnes = tableau_donnes.nextSiblingElement();
             tableau_donnes = tableau_donnes.nextSiblingElement();

             item->setText(1, tableau_donnes.text().right(tableau_donnes.text().size() - 9));

             fichierla.close();

             item->setTextAlignment(1, Qt::AlignRight);
             arbre->addTopLevelItem(item);

         }
    }
}
