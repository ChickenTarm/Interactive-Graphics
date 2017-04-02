#include <QApplication>
#include <QVBoxLayout>
#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include <iostream>
#include "img_viewer.h"
#include <iostream>
#include <assert.h>
#include "tiny_obj_loader.h"
#include "mat4.h"
#include "triangle.h"
#include "vec4.h"
#include "camera.h"
#include <fstream>
#include <string>
#include <cmath>
#include <stdlib.h>

#define CLAMP(x) (x> 255 ? 255: ((x) < 0 ? 0 : (x)))
#if 1
#define HERE printf("Arrived here: %s: %d\n", __FILE__, __LINE__)
#else
#define HERE
#endif

using namespace std;

ImageViewer::ImageViewer(QWidget *parent) :
  QMainWindow(parent), channels(0) {
    // Variable Instantions
    setStyleSheet("QMainWindow::separator{ width: 0px; height: 0px; }");
    renderedPic = (img_t *) malloc(sizeof(img_t));
    modifiedPic = (img_t *) malloc(sizeof(img_t));
    z_buf = (z_buffer *) malloc(sizeof(z_buffer));
    main = new QWidget();
    filterPane = new QDockWidget(this);
    filters = new QWidget(filterPane);
    imgShower = new QLabel(main);
    menu = new QMenuBar(main);
    applyBut = new QPushButton("&Apply", filters);
    cameraChangeBut = new QPushButton("&Change Camera", filters);
    resetCameraBut = new QPushButton("&Reset Camera", filters);
    resetImageBut = new QPushButton("&Reset Image", filters);
    blurBox = new QSpinBox(filters);
    stdBox = new QDoubleSpinBox(filters);
    imageAngleBox = new QDoubleSpinBox(filters);
    imageAngleBox -> setMaximum(360);
    leftBox = new QSpinBox(filters);
    leftBox -> setMaximum(99999999);
    rightBox = new QSpinBox(filters);
    rightBox -> setMaximum(99999999);
    upBox = new QSpinBox(filters);
    upBox -> setMaximum(99999999);
    downBox = new QSpinBox(filters);
    downBox -> setMaximum(99999999);
    forwardBox = new QSpinBox(filters);
    forwardBox -> setMaximum(99999999);
    backBox = new QSpinBox(filters);
    backBox -> setMaximum(99999999);
    horizontalBox = new QDoubleSpinBox(filters);
    horizontalBox -> setMaximum(360);
    verticalBox = new QDoubleSpinBox(filters);
    verticalBox -> setMaximum(360);
    horizontalFOVBox = new QDoubleSpinBox(filters);
    horizontalFOVBox -> setMaximum(179);
    verticalFOVBox = new QDoubleSpinBox(filters);
    verticalFOVBox -> setMaximum(179);
    drawBox = new QDoubleSpinBox(filters);
    drawBox -> setMaximum(9999);
    boxBlurCheck = new QCheckBox("&Box", filters);
    medianCheck = new QCheckBox("&Median", filters);
    guassianCheck = new QCheckBox("&Guassian", filters);
    sobelCheck = new QCheckBox("&Sobel", filters);
    flipCheck = new QCheckBox("&Flip", filters);
    flopCheck = new QCheckBox("&Flop", filters);
    transposeCheck = new QCheckBox("&Transpose", filters);
    grayCheck = new QCheckBox("&Gray Scale", filters);
    filterScroll = new QScrollArea(filterPane);
    filterScroll -> setWidgetResizable(true);
    firstRender = true;

    setCentralWidget(main);


    //Sets up the filter pane
    filterPane->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    QVBoxLayout *filterLayout = new QVBoxLayout();
    QLabel *blurLabel = new QLabel(filters);
    blurLabel->setText(tr("Blurs"));
    filterLayout->addWidget(blurLabel);

    //Creates Box Blur and Median
    QWidget *boxMedianBlur = new QWidget(filters);
    QHBoxLayout *boxMedianBlurLayout = new QHBoxLayout();
    boxMedianBlurLayout->addWidget(boxBlurCheck);
    boxMedianBlurLayout->addWidget(medianCheck);
    boxMedianBlur->setLayout(boxMedianBlurLayout);
    filterLayout->addWidget(boxMedianBlur);

    //Creates Guassian Blur
    QWidget *guassian = new QWidget(filters);
    QHBoxLayout *guassianLayout = new QHBoxLayout();
    QLabel *stdLabel = new QLabel(filters);
    stdLabel->setText(tr("Standard Deviation: "));
    guassianLayout->addWidget(guassianCheck);
    guassianLayout->addWidget(stdLabel);
    guassianLayout->addWidget(stdBox);
    guassian->setLayout(guassianLayout);
    filterLayout->addWidget(guassian);

    //Creates Blur Radius
    QWidget *blurRad = new QWidget(filters);
    QHBoxLayout *blurLaylout = new QHBoxLayout();
    QLabel *radLabel = new QLabel(filters);
    radLabel->setText(tr("Blur Radius: "));
    blurLaylout->addWidget(radLabel);
    blurLaylout->addWidget(blurBox);
    blurRad -> setLayout(blurLaylout);
    filterLayout->addWidget(blurRad);

    QLabel *transformLabel = new QLabel(filters);
    transformLabel->setText(tr("Transformations"));
    filterLayout->addWidget(transformLabel);

    //Flip Flop
    QWidget *flipFlop = new QWidget(filters);
    QHBoxLayout *ffLayout = new QHBoxLayout();
    ffLayout ->addWidget(flipCheck);
    ffLayout -> addWidget(flopCheck);
    flipFlop -> setLayout(ffLayout);
    filterLayout -> addWidget(flipFlop);

    //Transpose
    QWidget *transposer = new QWidget(filters);
    QHBoxLayout *transposeLayout = new QHBoxLayout();
    transposeLayout -> addWidget(transposeCheck);
    transposer -> setLayout(transposeLayout);
    filterLayout -> addWidget(transposer);

    //Sobel
    QWidget *sobely = new QWidget(filters);
    QHBoxLayout *sobelyLayout = new QHBoxLayout();
    sobelyLayout -> addWidget(sobelCheck);
    sobely -> setLayout(sobelyLayout);
    filterLayout -> addWidget(sobely);

    //Gray Scale
    QWidget *gray = new QWidget(filters);
    QHBoxLayout *grayLayout = new QHBoxLayout();
    grayLayout -> addWidget(grayCheck);
    gray -> setLayout(grayLayout);
    filterLayout -> addWidget(gray);

    //Rotate Image
    QWidget *rotateImage = new QWidget(filters);
    QHBoxLayout *rotateImageLayout = new QHBoxLayout();
    QLabel *rotateImageLabel = new QLabel(filters);
    rotateImageLabel -> setText("Rotate Image CC: ");
    rotateImageLayout -> addWidget(rotateImageLabel);
    rotateImageLayout -> addWidget(imageAngleBox);
    rotateImage -> setLayout(rotateImageLayout);
    filterLayout -> addWidget(rotateImage);

    filterLayout->addWidget(applyBut);
    filterLayout->addWidget(resetImageBut);

    //Creates Camera
    //Left movement
    QWidget *left = new QWidget(filters);
    QHBoxLayout *leftLayout = new QHBoxLayout();
    QLabel *leftLabel = new QLabel(filters);
    leftLabel -> setText(tr("Move Left: "));
    leftLayout -> addWidget(leftLabel);
    leftLayout -> addWidget(leftBox);
    left -> setLayout(leftLayout);
    filterLayout -> addWidget(left);

    //Right movement
    QWidget *right = new QWidget(filters);
    QHBoxLayout *rightLayout = new QHBoxLayout();
    QLabel *rightLabel = new QLabel(filters);
    rightLabel -> setText(tr("Move Right: "));
    rightLayout -> addWidget(rightLabel);
    rightLayout -> addWidget(rightBox);
    right -> setLayout(rightLayout);
    filterLayout -> addWidget(right);

    //Up movement
    QWidget *up = new QWidget(filters);
    QHBoxLayout *upLayout = new QHBoxLayout();
    QLabel *upLabel = new QLabel(filters);
    upLabel -> setText(tr("Move Up: "));
    upLayout -> addWidget(upLabel);
    upLayout -> addWidget(upBox);
    up -> setLayout(upLayout);
    filterLayout -> addWidget(up);

    //Down movement
    QWidget *down = new QWidget(filters);
    QHBoxLayout *downLayout = new QHBoxLayout();
    QLabel *downLabel = new QLabel(filters);
    downLabel -> setText(tr("Move Down: "));
    downLayout -> addWidget(downLabel);
    downLayout -> addWidget(downBox);
    down -> setLayout(downLayout);
    filterLayout -> addWidget(down);

    //Forward movement
    QWidget *forward = new QWidget(filters);
    QHBoxLayout *forwardLayout = new QHBoxLayout();
    QLabel *forwardLabel = new QLabel(filters);
    forwardLabel -> setText(tr("Move Forward: "));
    forwardLayout -> addWidget(forwardLabel);
    forwardLayout -> addWidget(forwardBox);
    forward -> setLayout(forwardLayout);
    filterLayout -> addWidget(forward);

    //Back movement
    QWidget *back = new QWidget(filters);
    QHBoxLayout *backLayout = new QHBoxLayout();
    QLabel *backLabel = new QLabel(filters);
    backLabel -> setText(tr("Move Back: "));
    backLayout -> addWidget(backLabel);
    backLayout -> addWidget(backBox);
    back -> setLayout(backLayout);
    filterLayout -> addWidget(back);

    //Horizontal Angle
    QWidget *horizontal = new QWidget(filters);
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QLabel *horizontalLabel = new QLabel(filters);
    horizontalLabel -> setText(tr("Rotate Horizontal: "));
    horizontalLayout -> addWidget(horizontalLabel);
    horizontalLayout -> addWidget(horizontalBox);
    horizontal -> setLayout(horizontalLayout);
    filterLayout -> addWidget(horizontal);

    //Vertical Angle
    QWidget *vertical = new QWidget(filters);
    QHBoxLayout *verticalLayout = new QHBoxLayout();
    QLabel *verticalLabel = new QLabel(filters);
    verticalLabel -> setText(tr("Rotate Vertical: "));
    verticalLayout -> addWidget(verticalLabel);
    verticalLayout -> addWidget(verticalBox);
    vertical -> setLayout(verticalLayout);
    filterLayout -> addWidget(vertical);

    //Horizontal FOV
    QWidget *horizontalFOV = new QWidget(filters);
    QHBoxLayout *horizontalFOVLayout = new QHBoxLayout();
    QLabel *horizontalFOVLabel = new QLabel(filters);
    horizontalFOVLabel -> setText("Horizontal FOV Angle: ");
    horizontalFOVLayout -> addWidget(horizontalFOVLabel);
    horizontalFOVLayout -> addWidget(horizontalFOVBox);
    horizontalFOV -> setLayout(horizontalFOVLayout);
    filterLayout -> addWidget(horizontalFOV);

    //Vertical FOV
    QWidget *verticalFOV = new QWidget(filters);
    QHBoxLayout *verticalFOVLayout = new QHBoxLayout();
    QLabel *verticalFOVLabel = new QLabel(filters);
    verticalFOVLabel -> setText("Vertical FOV Angle: ");
    verticalFOVLayout -> addWidget(verticalFOVLabel);
    verticalFOVLayout -> addWidget(verticalFOVBox);
    verticalFOV -> setLayout(verticalFOVLayout);
    filterLayout -> addWidget(verticalFOV);

    QWidget *drawDistance = new QWidget(filters);
    QHBoxLayout *drawLayout = new QHBoxLayout();
    QLabel *drawLabel = new QLabel(filters);
    drawLabel -> setText("Draw Distance: ");
    drawLayout -> addWidget(drawLabel);
    drawLayout -> addWidget(drawBox);
    drawDistance -> setLayout(drawLayout);
    filterLayout -> addWidget(drawDistance);

    filterLayout -> addWidget(cameraChangeBut);
    filterLayout -> addWidget(resetCameraBut);

    filterLayout->addStretch(1);
    filters->setLayout(filterLayout);
    filterScroll -> setWidget(filters);
    filterPane->setWidget(filterScroll);
    addDockWidget(Qt::LeftDockWidgetArea, filterPane);



    //Sets up the image pane
    imgShower->show();
    QDockWidget *imagePane = new QDockWidget(this);
    imagePane->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    imagePane -> setWidget(imgShower);
    addDockWidget(Qt::RightDockWidgetArea, imagePane);

    createFileActions();
    createMenu();

    connect(applyBut, SIGNAL(clicked(bool)), this, SLOT(apply()));
    connect(cameraChangeBut, SIGNAL(clicked(bool)), this, SLOT(changeCamera()));
    connect(resetCameraBut, SIGNAL(clicked(bool)), this, SLOT(resetCam()));
    connect(resetImageBut, SIGNAL(clicked(bool)), this, SLOT(resetImage()));
}

ImageViewer::~ImageViewer() {
  // delete rCheck;
  // delete gCheck;
  // delete bCheck;
    free(renderedPic->data);
    free(renderedPic);
    free(modifiedPic->data);
    free(modifiedPic);
}

void ImageViewer::createMenu() {
    fileMenu = new QMenu("File");
    fileMenu -> addAction(openOBJAct);
    fileMenu -> addAction(openCameraAct);
    fileMenu -> addAction(saveAsAct);
    fileMenu -> addAction(renderAct);
    menu -> addMenu(fileMenu);
}

void ImageViewer::createFileActions() {
    openOBJAct = new QAction(tr("&Open OBJ File"), this);
    openOBJAct -> setStatusTip(tr("Opens an existing .obj file"));
    connect(openOBJAct, SIGNAL(triggered(bool)), this, SLOT(openObj()));

    openCameraAct = new QAction(tr("&Open Camera File"), this);
    openCameraAct -> setStatusTip(tr("Opens an existing .txt file"));
    connect(openCameraAct, SIGNAL(triggered(bool)), this, SLOT(openCamera()));

    saveAsAct = new QAction(tr("&Save As ..."), this);
    saveAsAct -> setStatusTip(tr("Saves the output as a .ppm"));
    connect(saveAsAct, SIGNAL(triggered(bool)), this, SLOT(saveAs()));

    renderAct = new QAction(tr("&Render"), this);
    renderAct -> setStatusTip(tr("Renders the .obj file using the camera selected"));
    connect(renderAct, SIGNAL(triggered(bool)), this, SLOT(render()));

    loadImageAct = new QAction(tr("&Load"), this);
    loadImageAct -> setStatusTip(tr("Opens the Image selected"));
    connect(loadImageAct, SIGNAL(triggered(bool)), this, SLOT(load()));
}

void ImageViewer::openObj() {
    objName =  QFileDialog::getOpenFileName(this, tr("Open Obj"), QDir::homePath(), tr("Object Files(*.obj)"));
    firstRender = true;
}

void ImageViewer::openCamera() {
    cameraName =  QFileDialog::getOpenFileName(this, tr("Open Camera"), QDir::homePath(), tr("Text Files(*.txt)"));
    std::vector<float> param;
    std::string num;
    std::ifstream camFile(cameraName.toStdString().c_str());
    while (camFile >> num) {
        param.push_back(atof(num.c_str()));
    }
    camera c(param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7], param[8], param[9], param[10], param[11], param[12], param[13], param[14]);
    cam = c;
    camOrig = c;
    camRead = true;
    float horizontalLeg = abs(cam.frust.left - cam.frust.right) / 2;
    float verticalLeg = abs(cam.frust.top - cam.frust.bottom) / 2;
    float nearLeg = cam.frust.near;
    float horizontalFOVAngle = 2 * (atan(horizontalLeg / nearLeg) * 180 / M_PI);
    float verticalFOVAngle = 2 * (atan(verticalLeg / nearLeg) * 180 / M_PI);
    horizontalFOVBox -> setValue(horizontalFOVAngle);
    verticalFOVBox -> setValue(verticalFOVAngle);
    drawBox -> setValue(cam.frust.far - cam.frust.near);
}

void ImageViewer::saveAs() {
    QString saveName = QFileDialog::getSaveFileName(this, tr("Save As"), QDir::homePath(), tr("Image Files(*.ppm)"));
    FILE *output = fopen(saveName.toStdString().c_str(), "wb+");
    fprintf(output, "P6\n%d %d 255\n", renderedPic->w, renderedPic->h);
    for (int t = 0; t < renderedPic->w * renderedPic->h; t = t + 1) {
        fwrite(&(modifiedPic->data[t].r), sizeof(modifiedPic->data[t].r), 1, output);
        fwrite(&(modifiedPic->data[t].g), sizeof(modifiedPic->data[t].g), 1, output);
        fwrite(&(modifiedPic->data[t].b), sizeof(modifiedPic->data[t].b), 1, output);
    }
    fclose(outputTemp);
}

void ImageViewer::render() {
    bool widthOk;
    w = (float) QInputDialog::getInt(this, tr("Width"), tr("Width:"), 0, 0, 5000, 1, &widthOk);

    bool heightOk;
    h = (float) QInputDialog::getInt(this, tr("Height"), tr("Height:"), 0, 0, 5000, 1, &heightOk);

    QStringList options;
    options << tr("Diffuse") << tr("White") << tr("Flat Normals") << tr("Gouraud Shading") << tr("Barycentric Shading") << tr("Perspective Correct Gouraud Shading") << tr("Perspective Correct Barycentric Shading");
    bool optionsOk;
    QString shading = QInputDialog::getItem(this, tr("Shading Selection"), tr("Shading:"), options, 0, false, &optionsOk);

    z_method = 0;
    color_method = 0;

    if (!strcmp(shading.toStdString().c_str(), "White")) {
        color_method = 1;
    }
    else if (!strcmp(shading.toStdString().c_str(), "Flat Normals")) {
        color_method = 2;
    }
    else if (!strcmp(shading.toStdString().c_str(), "Gouraud Shading")) {
        color_method = 3;
    }
    else if (!strcmp(shading.toStdString().c_str(), "Barycentric Shading")) {
        color_method = 4;
    }
    else if (!strcmp(shading.toStdString().c_str(), "Perspective Correct Gouraud Shading")) {
        z_method = 1;
        color_method = 5;
    }
    else {
        z_method = 2;
        color_method = 6;
    }

    if (renderedPic->w != ((int) w) || renderedPic->h != ((int) h)) {
        if (!firstRender) {
            free(renderedPic->data);
            free(modifiedPic->data);
            free(z_buf->data);
        }
        firstRender = false;
        renderedPic->w = w;
        renderedPic->h = h;
        renderedPic->data = (pixel_t *) calloc(w * h, sizeof(pixel_t));
        modifiedPic->w = w;
        modifiedPic->h = h;
        modifiedPic->data = (pixel_t *) calloc(w * h, sizeof(pixel_t));
        z_buf->data = (float *) malloc(w * h * sizeof(float));
        z_buf->w = w;
        z_buf->h = h;
    }

    PixelCalc();
}

void ImageViewer::load() {
    QString imageName =  QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::homePath(), tr("Image Files(*.ppm)"));
    img.load(imageName);
    imgShower->setPixmap(img);
}

void ImageViewer::apply() {
    if (boxBlurCheck->isChecked()) {
        boxBlur();
    }
    if (medianCheck->isChecked()) {
        medianBlur();
    }
    if (guassianCheck->isChecked()) {
        guassianBlur();
    }
    if (flipCheck->isChecked()) {
        flip();
    }
    if (flopCheck->isChecked()) {
        flop();
    }
    if (transposeCheck->isChecked()) {
        transpose();
    }
    if (sobelCheck->isChecked()) {
        sobel();
    }
    if (grayCheck -> isChecked()) {
        grayScale();
    }
    if (imageAngleBox->value() > .001) {
        rotate(imageAngleBox->value());
    }
    boxBlurCheck -> setCheckState(Qt::Unchecked);
    medianCheck -> setCheckState(Qt::Unchecked);
    guassianCheck -> setCheckState(Qt::Unchecked);
    flipCheck -> setCheckState(Qt::Unchecked);
    flopCheck -> setCheckState(Qt::Unchecked);
    transposeCheck -> setCheckState(Qt::Unchecked);
    sobelCheck -> setCheckState(Qt::Unchecked);
    grayCheck -> setCheckState(Qt::Unchecked);
    imageAngleBox -> setValue(0);
    outputTemp = fopen(filePath.toStdString().c_str(), "wb+");
    fprintf(outputTemp, "P6\n%d %d 255\n", modifiedPic->w, modifiedPic->h);
    for (int t = 0; t < modifiedPic->w * modifiedPic->h; t = t + 1) {
        fwrite(&(modifiedPic->data[t].r), sizeof(modifiedPic->data[t].r), 1, outputTemp);
        fwrite(&(modifiedPic->data[t].g), sizeof(modifiedPic->data[t].g), 1, outputTemp);
        fwrite(&(modifiedPic->data[t].b), sizeof(modifiedPic->data[t].b), 1, outputTemp);
    }
    fclose(outputTemp);
    img.load(filePath);
    imgShower->setPixmap(img);
}

void ImageViewer::changeCamera() {
    vec4 oldLoc = cam.location;
    vec4 xDir = cam.x;
    vec4 yDir = cam.y;
    vec4 zDir = cam.z;
    int leftStep = leftBox->value();
    int rightStep = rightBox->value();
    int upStep = upBox->value();
    int downStep = downBox->value();
    int forwardStep = forwardBox->value();
    int backStep = backBox->value();

    float newX = oldLoc[0] - leftStep * xDir[0] + rightStep * xDir[0] + upStep * yDir[0] - downStep * yDir[0] + forwardStep * zDir[0] -backStep * zDir[0];
    float newY = oldLoc[1] - leftStep * xDir[1] + rightStep * xDir[1] + upStep * yDir[1] - downStep * yDir[1] + forwardStep * zDir[1] -backStep * zDir[1];
    float newZ = oldLoc[2] - leftStep * xDir[2] + rightStep * xDir[2] + upStep * yDir[2] - downStep * yDir[2] + forwardStep * zDir[2] -backStep * zDir[2];

    vec4 newLocation(newX, newY, newZ, 0);

    double horizontalAngle = horizontalBox->value();
    double verticalAngle = verticalBox->value();

    mat4 horizontalRot = mat4::rot(horizontalAngle, camOrig.y[0], camOrig.y[1], camOrig.y[2]);
    mat4 verticalRot = mat4::rot(verticalAngle, cam.x[0], cam.x[1], cam.x[2]);

    vec4 newXDir = horizontalRot * cam.x;
    vec4 newYDir = verticalRot * cam.y;
    vec4 newZDir = verticalRot * horizontalRot * cam.z;

    camera c(camOrig.frust, newLocation, newZDir, newYDir, newXDir);

    cam = c;

    float horizontalFOV = ((horizontalFOVBox -> value()) / 2.0) / 180.0 * M_PI;
    float verticalFOV = ((verticalFOVBox -> value()) / 2.0) / 180.0 * M_PI;

    float horizontalDist = cam.frust.near * tan(horizontalFOV);
    float verticalDist = cam.frust.near * tan(verticalFOV);


    if (cam.frust.left < 0) {
        cam.frust.left = -1 * horizontalDist;
        cam.frust.right = horizontalDist;
    }
    else {
        cam.frust.left = horizontalDist;
        cam.frust.right = -1 * horizontalDist;
    }
    if (cam.frust.top < 0) {
        cam.frust.top = -1 * verticalDist;
        cam.frust.bottom = verticalDist;
    }
    else {
        cam.frust.top = verticalDist;
        cam.frust.bottom = -1 * verticalDist;
    }

    float horizontalLeg = abs(cam.frust.left - cam.frust.right) / 2;
    float verticalLeg = abs(cam.frust.top - cam.frust.bottom) / 2;
    float nearLeg = cam.frust.near;
    float horizontalFOVAngle = 2 * (atan(horizontalLeg / nearLeg) * 180 / M_PI);
    float verticalFOVAngle = 2 * (atan(verticalLeg / nearLeg) * 180 / M_PI);
    horizontalFOVBox -> setValue(horizontalFOVAngle);
    verticalFOVBox -> setValue(verticalFOVAngle);

    cam.frust.far = cam.frust.near + drawBox -> value();

    cam.updateProjectMat();

    if (firstRender) {
        render();
        apply();
    }
    else {
        PixelCalc();
        apply();
    }
}

void ImageViewer::resetCam() {
    if (firstRender) {
        render();
        apply();
    }
    else {
        cam = camOrig;
        PixelCalc();
        apply();
    }
}

void ImageViewer::resetImage() {
    if (!firstRender) {
        w = renderedPic->w;
        h = renderedPic->h;
        free(modifiedPic->data);
        modifiedPic->w = w;
        modifiedPic->h = h;
        modifiedPic->data = (pixel_t *) calloc(w * h, sizeof(pixel_t));
        PixelCalc();
    }
}

std::vector<triangle> ImageViewer::toNDC(camera &c, std::vector<triangle> triangles) {
    std::vector<triangle> temp;
    for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
        triangle &tri = triangles[k];
        tri[0] = c.transform(tri[0]);
        tri[0] = tri[0] / tri[0][3];
        tri[1] = c.transform(tri[1]);
        tri[1] = tri[1] / tri[1][3];
        tri[2] = c.transform(tri[2]);
        tri[2] = tri[2] / tri[2][3];
        temp.push_back(tri);
    }
    return temp;
}

std::vector<triangle> ImageViewer::toScreen(float width, float height, std::vector<triangle> triangles) {
    std::vector<triangle> temp;
        for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
            triangle &tri = triangles[k];
            tri[0][0] = roundf((tri[0][0] + 1) * width / 2);
            tri[0][1] = roundf((1 - tri[0][1]) * height / 2);
            tri[1][0] = roundf((tri[1][0] + 1) * width / 2);
            tri[1][1] = roundf((1 - tri[1][1]) * height / 2);
            tri[2][0] = roundf((tri[2][0] + 1) * width / 2);
            tri[2][1] = roundf((1 - tri[2][1]) * height / 2);
            temp.push_back(tri);
        }
        return temp;
}

void ImageViewer::boundBox(triangle &t, int width, int height) {
    int minx = min(min(t[0][0], t[1][0]), t[2][0]);
    minx = max(0, minx);
    int maxx = max(max(t[0][0], t[1][0]), t[2][0]);
    maxx = min(width, maxx);
    int miny = min(min(t[0][1], t[1][1]), t[2][1]);
    miny = max(0, miny);
    int maxy = max(max(t[0][1], t[1][1]), t[2][1]);
    maxy = min(height, maxy);
    t.setBounds(minx, maxx, miny, maxy);
}

std::vector<triangle> ImageViewer::visibility(std::vector<triangle> triangles, int width, int height) {
    std::vector<triangle> temp;
    for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
        float z1 = triangles[k][0][2];
        float z2 = triangles[k][1][2];
        float z3 = triangles[k][2][2];
        if ((z1 < 0 || z1 > 1) && (z2 < 0 || z2 > 1) && (z3 < 0 || z3 > 1)) {
            continue;
        }
        else {
            float x1 = triangles[k][0][0];
            float x2 = triangles[k][1][0];
            float x3 = triangles[k][2][0];
            float y1 = triangles[k][0][1];
            float y2 = triangles[k][1][1];
            float y3 = triangles[k][2][1];
            bool out_x = (x1 < 0 || x1 > width) && (x2 < 0 || x2 > width) && (x3 < 0 || x3 > width);
            bool out_y = (y1 < 0 || y1 > height) && (y2 < 0 || y2 > height) && (y3 < 0 || y3 > height);
            if (out_x || out_y) {
                continue;
            }
            else {
                boundBox(triangles[k], width, height);
                temp.push_back(triangles[k]);
            }
        }
    }
    return temp;
}

void ImageViewer::PixelCalc() {
    //Reads and parses .obj file
    LoadObj(shapes, materials, objName.toStdString().c_str(), NULL);
    for (int j = 0; j < ((int) shapes.size()); j = j + 1) {
        for (int i = 0; i < ((int) shapes[j].mesh.indices.size()); i = i + 3) {
            int index1 = shapes[j].mesh.indices[i] * 3;
            int index2 = shapes[j].mesh.indices[i + 1] * 3;
            int index3 = shapes[j].mesh.indices[i + 2] * 3;
            vec4 vertex1(shapes[j].mesh.positions[index1], shapes[j].mesh.positions[index1 + 1], shapes[j].mesh.positions[index1 + 2], 1);
            vec4 vertex2(shapes[j].mesh.positions[index2], shapes[j].mesh.positions[index2 + 1], shapes[j].mesh.positions[index2 + 2], 1);
            vec4 vertex3(shapes[j].mesh.positions[index3], shapes[j].mesh.positions[index3 + 1], shapes[j].mesh.positions[index3 + 2], 1);
            vec4 n1(shapes[j].mesh.normals[index1], shapes[j].mesh.normals[index1 + 1], shapes[j].mesh.normals[index1 + 2], 0);
            vec4 n2(shapes[j].mesh.normals[index2], shapes[j].mesh.normals[index2 + 1], shapes[j].mesh.normals[index2 + 2], 0);
            vec4 n3(shapes[j].mesh.normals[index3], shapes[j].mesh.normals[index3 + 1], shapes[j].mesh.normals[index3 + 2], 0);
            int mat_id = shapes[j].mesh.material_ids[i / 3];
            vec4 diffuse(materials[mat_id].diffuse[0], materials[mat_id].diffuse[1], materials[mat_id].diffuse[2], 0);
            triangle t(vertex1, vertex2, vertex3, n1, n2, n3, diffuse);
            triangles.push_back(t);
        }
    }

    //Changes to NDC and rotates
    triangles = toNDC(cam, triangles);
    for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
        triangles[k].rotateNorms(cam);
    }

    //Sets width and height and computes bounding boxes
    for (int z = 0; z < z_buf->w * z_buf->h; z = z + 1) {
        z_buf->data[z] = 2;
    }
    triangles = toScreen(renderedPic->w, renderedPic->h, triangles);
    triangles = visibility(triangles, renderedPic->w - 1, renderedPic->h - 1);
    for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
        triangles[k].getPixels();
    }

    for (int t = 0; t < renderedPic->w * renderedPic->h; t = t + 1) {
        renderedPic->data[t].r = 0;
        renderedPic->data[t].g = 0;
        renderedPic->data[t].b = 0;
        modifiedPic->data[t].r = 0;
        modifiedPic->data[t].g = 0;
        modifiedPic->data[t].b = 0;
    }

    //Interpolates colors and z
    for (int k = 0; k < ((int) triangles.size()); k = k + 1) {
        int range = triangles[k].max_y - triangles[k].min_y;
        for (int r = 0; r < range; r = r + 1) {
            int sx = triangles[k].getStartx(r);
            int ex = triangles[k].getEndx(r);
            float y = triangles[k].getY(r);
            int py = y - .5;
            for (int p = sx; p <= ex; p = p + 1) {
                int index = p + py * renderedPic->w;
                float z_norm = triangles[k].getZ(sx, ex, y, p, z_method);
                if (z_norm >= 0 && z_norm <= 1) {
                    float zb = z_buf->data[index];
                    if (z_norm < zb) {
                        z_buf->data[index] = z_norm;
                        //color pixel
                        vec4 color = triangles[k].getColor(sx, ex, y, p, color_method);
                        renderedPic->data[index].r = color[0];
                        renderedPic->data[index].g = color[1];
                        renderedPic->data[index].b = color[2];
                        modifiedPic->data[index].r = color[0];
                        modifiedPic->data[index].g = color[1];
                        modifiedPic->data[index].b = color[2];
                    }
                }
            }
        }
        free(triangles[k].pix);
    }

    filePath = QDir::homePath() + "/jfoi2f2nq8fo2nsadf8923j.ppm";
    outputTemp = fopen(filePath.toStdString().c_str(), "wb+");
    fprintf(outputTemp, "P6\n%d %d 255\n", renderedPic->w, renderedPic->h);

    for (int t = 0; t < renderedPic->w * renderedPic->h; t = t + 1) {
        fwrite(&(renderedPic->data[t].r), sizeof(renderedPic->data[t].r), 1, outputTemp);
        fwrite(&(renderedPic->data[t].g), sizeof(renderedPic->data[t].g), 1, outputTemp);
        fwrite(&(renderedPic->data[t].b), sizeof(renderedPic->data[t].b), 1, outputTemp);
    }
    fclose(outputTemp);
    unsigned char *start = &renderedPic->data[0].r;
    cout << "Start: " << start << endl;

    img.load(filePath);
//    img.loadFromData(&renderedPic->data[0].r, renderedPic->w * renderedPic->h * 3, "PPM");
    imgShower->setPixmap(img);
}

void ImageViewer::boxBlur() {
    img_t *alt = (img_t*) malloc(sizeof(img_t));
    alt->w = modifiedPic->w;
    alt->h = modifiedPic->h;
    alt->data = (pixel_t*) calloc(modifiedPic->w * modifiedPic->h, sizeof(pixel_t));
    int rad = blurBox->value();
    int row;
    int col;
    int w = this->w;
    int h = this->h;
    for (int j = 0; j < w * h; j = j + 1) {
        row  = j / w;
        col = j % w;
        if (row < rad || (h - 1) - row < rad || col < rad || (w - 1) - col < rad) {
            alt->data[j].b = 0;
            alt->data[j].r = 0;
            alt->data[j].g = 255;
        }
        else {
            int all_red = 0;
            int all_green = 0;
            int all_blue = 0;
            for (int r = row - rad; r < row + rad + 1; r = r + 1) {
                for (int c = col - rad; c < col + rad + 1; c = c + 1) {
                    int index = w * r + c;
                    all_red = all_red + modifiedPic->data[index].r;
                    all_green = all_green + modifiedPic->data[index].g;
                    all_blue = all_blue + modifiedPic->data[index].b;
                }
            }
                float num = (2 * rad + 1) * (2 * rad + 1);
                unsigned char red = (unsigned char) CLAMP(all_red / num);
                unsigned char green = (unsigned char) CLAMP(all_green / num);
                unsigned char blue = (unsigned char) CLAMP(all_blue / num);
                alt->data[j].b = blue;
                alt->data[j].r = red;
                alt->data[j].g = green;
        }
    }
    img_t *temp = modifiedPic;
    temp->data = modifiedPic->data;
    modifiedPic = alt;
    modifiedPic->data = alt->data;
    free(temp->data);
    free(temp);
}

unsigned char ImageViewer::getMed(unsigned char *lst, int num) {
    for (int q = 0; q < num - 1; q = q + 1) {
        int min_index = q;
        for (int u = q + 1; u < num; u = u + 1) {
            if (lst[u] < lst[min_index]) {
                min_index = u;
            }
        }
        if (q != min_index) {
            unsigned char hold = lst[q];
            lst[q] = lst[min_index];
            lst[min_index] = hold;
        }
    }
    if (num % 2 == 0) {
        double combine = lst[num / 2] + lst[num / 2 + 1];
        return CLAMP(combine / 2.0);
    }
    else {
        return lst[num / 2];
    }
}

void ImageViewer::medianBlur() {
    img_t *alt = (img_t*) malloc(sizeof(img_t));
    alt->w = modifiedPic->w;
    alt->h = modifiedPic->h;
    alt->data = (pixel_t*) calloc(modifiedPic->w * modifiedPic->h, sizeof(pixel_t));
    int rad = blurBox->value();
    int row;
    int col;
    int area = (2 * rad + 1) * (2 * rad + 1);
    unsigned char reds[area];
    unsigned char greens[area];
    unsigned char blues[area];
    int w = this->w;
    int h = this->h;
    for (int j = 0; j < w * h; j = j + 1) {
        row  = j / w;
        col = j % w;
        if (row < rad || (h - 1) - row < rad || col < rad || (w - 1) - col < rad) {
            alt->data[j].b = 0;
            alt->data[j].r = 0;
            alt->data[j].g = 255;
        }
        else {
            int count = 0;
            for (int r = row - rad; r < row + rad + 1; r = r + 1) {
                for (int c = col - rad; c < col + rad + 1; c = c + 1) {
                    int index = w * r + c;
                    reds[count] =  modifiedPic->data[index].r;
                    greens[count] = modifiedPic->data[index].g;
                    blues[count] = modifiedPic->data[index].b;
                    count = count + 1;
                }
            }
            alt->data[j].b = getMed(blues, area);
            alt->data[j].r = getMed(reds, area);
            alt->data[j].g = getMed(greens, area);
        }
    }
    img_t *temp = modifiedPic;
    temp->data = modifiedPic->data;
    modifiedPic = alt;
    modifiedPic->data = alt->data;
    free(temp->data);
    free(temp);
}

void ImageViewer::guassianBlur() {
    img_t *alt = (img_t*) malloc(sizeof(img_t));
    alt->w = modifiedPic->w;
    alt->h = modifiedPic->h;
    alt->data = (pixel_t*) calloc(modifiedPic->w * modifiedPic->h, sizeof(pixel_t));
    int rad = blurBox->value();
    double s = stdBox->value();
    int row;
    int col;
    int w = this->w;
    int h = this->h;
    dub_i *mid = (dub_i*) malloc(sizeof(dub_i));
    mid->w = w;
    mid->h = h;
    mid->data = (dub_p*) calloc(w * h, sizeof(dub_p));
    for (int j = 0; j < w * h; j = j + 1) {
        row  = j / w;
        col = j % w;
        if (col < rad || (w - 1) - col < rad) {
            alt->data[j].b = 0;
            alt->data[j].g = 255;
            alt->data[j].r = 0;
        }
        else {
            double all_red = 0;
            double all_green = 0;
            double all_blue = 0;
            double num = 0;
            for (int p = col - rad; p < col + rad + 1; p = p + 1) {
                int index = row * w + p;
                double x = p - col;
                double weight = exp(-1 * x * x / (2 * s * s));
                num = num + weight;
                all_red = all_red + modifiedPic->data[index].r * weight;
                all_green = all_green + modifiedPic->data[index].g * weight;
                all_blue = all_blue + modifiedPic->data[index].b * weight;
            }
            double red = all_red / num;
            double green = all_green / num;
            double blue = all_blue / num;
            mid->data[j].b = blue;
            mid->data[j].r = red;
            mid->data[j].g = green;
        }
    }
    for (int j = 0; j < w * h; j = j + 1) {
        row  = j / w;
        col = j % w;
        if (row < rad || (h - 1) - row < rad || col < rad || (w - 1) - col < rad) {
            alt->data[j].b = 0;
            alt->data[j].g = 255;
            alt->data[j].r = 0;
        }
        else {
            double all_red = 0;
            double all_green = 0;
            double all_blue = 0;
            double num = 0;
            for (int p = row - rad; p < row + rad + 1; p = p + 1) {
                int index = p * w + col;
                double x = p - row;
                double weight = exp(-1 * x * x / (2 * s * s));
                num = num + weight;
                all_red = all_red + mid->data[index].r * weight;
                all_green = all_green + mid->data[index].g * weight;
                all_blue = all_blue + mid->data[index].b * weight;
            }
            unsigned char red = (unsigned char) CLAMP(all_red / num);
            unsigned char green = (unsigned char) CLAMP(all_green / num);
            unsigned char blue = (unsigned char) CLAMP(all_blue / num);
            alt->data[j].b = blue;
            alt->data[j].r = red;
            alt->data[j].g = green;
        }
    }
    free(mid->data);
    free(mid);
    img_t *temp = modifiedPic;
    temp->data = modifiedPic->data;
    modifiedPic = alt;
    modifiedPic->data = alt->data;
    free(temp->data);
    free(temp);
}

void ImageViewer::sobel() {
    int gx[9];
    gx[0] = gx[6]= -1;
    gx[1] = gx[4] = gx[7] = 0;
    gx[2] = gx[8] = 1;
    gx[3] = -2;
    gx[5] = 2;
    int gy[9];
    gy[0] = gy[2] = 1;
    gy[3] = gy[4] = gy[5] = 0;
    gy[6] = gy[8] = -1;
    gy[1] = 2;
    gy[7] = -2;
    img_t *alt = (img_t*) malloc(sizeof(img_t));
    alt->w = w;
    alt->h = h;
    alt->data = (pixel_t*) calloc(w * h, sizeof(pixel_t));
    img_t *gray = (img_t*) malloc(sizeof(img_t));
    gray->h = h;
    gray->w = w;
    gray->data = (pixel_t*) calloc(w * h, sizeof(pixel_t));
    for (int j = 0; j < w * h; j = j + 1) {
        unsigned char y = (unsigned char) CLAMP(.299 * modifiedPic->data[j].r + .587 * modifiedPic->data[j].g + .114 * modifiedPic->data[j].b);
        gray->data[j].r = y;
        gray->data[j].g = y;
        gray->data[j].b = y;
    }
    int w = this->w;
    int h = this->h;
    int row;
    int col;
    for (int j = 0; j < w * h; j = j + 1) {
        row  = j / w;
        col = j % w;
        if (row < 1 || (h - 1) - row < 1 || col < 1 || (w - 1) - col < 1) {
            alt->data[j].b = 0;
            alt->data[j].r = 0;
            alt->data[j].g = 255;
        }
        else {
            int x_grad = 0;
            int y_grad = 0;
            int count = 0;
            for (int r = row - 1; r < row + 2; r = r + 1) {
                for (int c = col - 1; c < col + 2; c = c + 1) {
                    int index = w * r + c;
                    x_grad = x_grad + gx[count] * gray->data[index].r;
                    y_grad = y_grad + gy[count] * gray->data[index].r;
                    count = count + 1;
                }
            }
            unsigned char g = (unsigned char) CLAMP(sqrt((double) (x_grad * x_grad + y_grad * y_grad)));
            alt->data[j].b = alt->data[j].r = alt->data[j].g = g;
        }
    }
    free(gray->data);
    free(gray);
    img_t *temp = modifiedPic;
    temp->data = modifiedPic->data;
    modifiedPic = alt;
    modifiedPic->data = alt->data;
    free(temp->data);
    free(temp);
}

void ImageViewer::grayScale() {
    img_t *alt = (img_t*) malloc(sizeof(img_t));
    alt->w = w;
    alt->h = h;
    alt->data = (pixel_t*) calloc(w * h, sizeof(pixel_t));
    for (int j = 0; j < w * h; j = j + 1) {
        unsigned char y = (unsigned char) CLAMP(.299 * modifiedPic->data[j].r + .587 * modifiedPic->data[j].g + .114 * modifiedPic->data[j].b);
        alt->data[j].r = y;
        alt->data[j].g = y;
        alt->data[j].b = y;
    }
    img_t *temp = modifiedPic;
    temp->data = modifiedPic->data;
    modifiedPic = alt;
    modifiedPic->data = alt->data;
    free(temp->data);
    free(temp);
}

void ImageViewer::flip() {
    img_t *alt = (img_t*) malloc(sizeof(img_t));
    alt->w = w;
    alt->h = h;
    alt->data = (pixel_t*) calloc(w * h, sizeof(pixel_t));
    int row;
    int col;
    int flip;
    int w = this->w;
    int h = this->h;
    for (int j = 0; j < w * h; j = j + 1) {
        row  = j / w;
        col = j % w;
        flip = (w - 1 - col) + w * row;
        alt->data[flip] = modifiedPic->data[j];
    }
    img_t *temp = modifiedPic;
    temp->data = modifiedPic->data;
    modifiedPic = alt;
    modifiedPic->data = alt->data;
    free(temp->data);
    free(temp);
}

void ImageViewer::flop() {
    img_t *alt = (img_t*) malloc(sizeof(img_t));
    alt->w = w;
    alt->h = h;
    alt->data = (pixel_t*) calloc(w * h, sizeof(pixel_t));
    int row;
    int col;
    int flip;
    int w = this->w;
    int h = this->h;
    for (int j = 0; j < w * h; j = j + 1) {
        row  = j / w;
        col = j % w;
        flip = w * (h - 1 - row) + col;
        alt->data[flip] = modifiedPic->data[j];
    }
    img_t *temp = modifiedPic;
    temp->data = modifiedPic->data;
    modifiedPic = alt;
    modifiedPic->data = alt->data;
    free(temp->data);
    free(temp);
}

void ImageViewer::transpose() {
    img_t *alt = (img_t*) malloc(sizeof(img_t));
    alt->w = h;
    alt->h = w;
    alt->data = (pixel_t*) calloc(w * h, sizeof(pixel_t));
    int row;
    int col;
    int flip;
    int w = this->w;
    int h = this->h;
    for (int j = 0; j < w * h; j = j + 1) {
        row  = j / w;
        col = j % w;
        flip = h * col + row;
        alt->data[flip] = modifiedPic->data[j];
    }
    this->h = alt->h;
    this->w = alt->w;
    img_t *temp = modifiedPic;
    temp->data = modifiedPic->data;
    modifiedPic = alt;
    modifiedPic->data = alt->data;
    free(temp->data);
    free(temp);
}

void ImageViewer::rotate(double cc_angle) {
    double to_rad = M_PI / 180.0;
    cc_angle = cc_angle * to_rad;
    double cw_angle = -1 * cc_angle;
    double pos_x = 0;
    double neg_x = 0;
    double pos_y = 0;
    double neg_y = 0;
    int mid_x = (w - 1) / 2;
    int mid_y = (h - 1) / 2;
    int w = this->w;
    int h = this->h;
    for (int j = 0; j < w * h; j = j + 1) {
        int y  = mid_y - j / w;
        int x = j % w - mid_x;
        double nx = x * cos(cc_angle) - y * sin(cc_angle);
        double ny = x * sin(cc_angle) + y * cos(cc_angle);
        if (nx > 0) {
            if (pos_x < nx) {
                pos_x = nx;
            }
        }
        else {
            if (neg_x > nx) {
                neg_x = nx;
            }
        }
        if (ny > 0) {
            if (pos_y < ny) {
                pos_y = ny;
            }
        }
        else {
            if (neg_y > ny) {
                neg_y = ny;
            }
        }
    }
    int nw = round(pos_x - neg_x);
    int nh = round(pos_y - neg_y);
    img_t *alt = (img_t*) malloc(sizeof(img_t));
    alt->w = nw;
    alt->h = nh;
    alt->data = (pixel_t*) calloc(nw * nh, sizeof(pixel_t));
    int nmid_x = nw / 2;
    int nmid_y = nh / 2;
    for (int j = 0; j < nw * nh; j = j + 1) {
        int x = j % nw - nmid_x;
        int y = nmid_y - j / nw;
        double nx = x * cos(cw_angle) - y * sin(cw_angle);
        double ny = x * sin(cw_angle) + y * cos(cw_angle);
        if (nx >= 0) {
            if (round(nx) > mid_x) {
                alt->data[j].r = alt->data[j].b = 0;
                alt->data[j].g = 225;
                continue;
            }
        }
        else {
            if (round(nx) < -1 * mid_x) {
                alt->data[j].r = alt->data[j].b = 0;
                alt->data[j].g = 225;
                continue;
            }
        }
        if (ny >= 0) {
            if (round(ny) > mid_y) {
                alt->data[j].r = alt->data[j].b = 0;
                alt->data[j].g = 225;
                continue;
            }
        }
        else {
            if (round(ny) < -1 * mid_y) {
                alt->data[j].r = alt->data[j].b = 0;
                alt->data[j].g = 225;
                continue;
            }
        }
        int col = round(nx) + mid_x;
        int row = mid_y - round(ny);
        double r_top = 0;
        double g_top = 0;
        double b_top = 0;
        if (nx >= 0 && ny >= 0) {
            int col_bord;
            if (w - 1 > col + 1) {
                col_bord = col + 1;
            }
            else {
                col_bord = w - 1;
            }
            int row_bord;
            if (0 < row - 1) {
                row_bord = row - 1;
            }
            else {
                row_bord = 0;
            }
            if (row == row_bord && col == col_bord) {
                alt->data[j] = modifiedPic->data[row * w + col];
                continue;
            }
            if (row == row_bord) {
                int index = w * row + col;
                r_top = modifiedPic->data[index].r + (nx - (int) nx) / 2.0 * (modifiedPic->data[index + 1].r - modifiedPic->data[index].r);
                g_top = modifiedPic->data[index].g + (nx - (int) nx) / 2.0 * (modifiedPic->data[index + 1].g - modifiedPic->data[index].g);
                b_top = modifiedPic->data[index].b + (nx - (int) nx) / 2.0 * (modifiedPic->data[index + 1].b - modifiedPic->data[index].b);
                alt->data[j].r = (unsigned char) CLAMP(r_top);
                alt->data[j].g = (unsigned char) CLAMP(g_top);
                alt->data[j].b = (unsigned char) CLAMP(b_top);
                continue;
            }
            if (col == col_bord) {
                int index = w * row + col;
                int index2 = w * (row_bord) + col;
                r_top = modifiedPic->data[index].r + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].r - modifiedPic->data[index].r);
                g_top = modifiedPic->data[index].g + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].g - modifiedPic->data[index].g);
                b_top = modifiedPic->data[index].b + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].b - modifiedPic->data[index].b);
                alt->data[j].r = (unsigned char) CLAMP(r_top);
                alt->data[j].g = (unsigned char) CLAMP(g_top);
                alt->data[j].b = (unsigned char) CLAMP(b_top);
                continue;
            }
            double x_dist = fabs(round(nx) - nx);
            double y_dist = fabs(round(ny) - ny);
            r_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].r + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].r + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].r + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].r;
            g_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].g + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].g + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].g + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].g;
            b_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].b + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].b + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].b + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].b;
            alt->data[j].r = (unsigned char) CLAMP(r_top);
            alt->data[j].g = (unsigned char) CLAMP(g_top);
            alt->data[j].b = (unsigned char) CLAMP(b_top);
            continue;
        }
        if (nx < 0  && ny > 0) {
            int col_bord;
            if (0 < col - 1) {
                col_bord = col - 1;
            }
            else {
                col_bord = 0;
            }
            int row_bord;
            if (0 < row - 1) {
                row_bord = row - 1;
            }
            else {
                row_bord = 0;
            }
            if (row == row_bord && col == col_bord) {
                alt->data[j] = modifiedPic->data[row * w + col];
                continue;
            }
            if (row == row_bord) {
                int index = w * row + col;
                r_top = modifiedPic->data[index + 1].r + (nx - (int) nx) / 2.0 * (modifiedPic->data[index].r - modifiedPic->data[index + 1].r);
                g_top = modifiedPic->data[index + 1].g + (nx - (int) nx) / 2.0 * (modifiedPic->data[index].g - modifiedPic->data[index + 1].g);
                b_top = modifiedPic->data[index + 1].b + (nx - (int) nx) / 2.0 * (modifiedPic->data[index].b - modifiedPic->data[index + 1].b);
                alt->data[j].r = (unsigned char) CLAMP(r_top);
                alt->data[j].g = (unsigned char) CLAMP(g_top);
                alt->data[j].b = (unsigned char) CLAMP(b_top);
                continue;
            }
            if (col == col_bord) {
                int index = w * row + col;
                int index2 = w * (row_bord) + col;
                r_top = modifiedPic->data[index].r + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].r - modifiedPic->data[index].r);
                g_top = modifiedPic->data[index].g + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].g - modifiedPic->data[index].g);
                b_top = modifiedPic->data[index].b + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].b - modifiedPic->data[index].b);
                alt->data[j].r = (unsigned char) CLAMP(r_top);
                alt->data[j].g = (unsigned char) CLAMP(g_top);
                alt->data[j].b = (unsigned char) CLAMP(b_top);
                continue;
            }
            double x_dist = fabs(round(nx) - nx);
            double y_dist = fabs(round(ny) - ny);
            r_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].r + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].r + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].r + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].r;
            g_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].g + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].g + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].g + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].g;
            b_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].b + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].b + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].b + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].b;
            alt->data[j].r = (unsigned char) CLAMP(r_top);
            alt->data[j].g = (unsigned char) CLAMP(g_top);
            alt->data[j].b = (unsigned char) CLAMP(b_top);
            continue;
        }
        if (nx < 0  && ny < 0) {
            int col_bord;
            if (0 < col - 1) {
                col_bord = col - 1;
            }
            else {
                col_bord = 0;
            }
            int row_bord;
            if (h - 1 > row + 1) {
                row_bord = row + 1;
            }
            else {
                row_bord = h - 1;
            }
            if (row == row_bord && col == col_bord) {
                alt->data[j] = modifiedPic->data[row * w + col];
                continue;
            }
            if (row == row_bord) {
                int index = w * row + col;
                r_top = modifiedPic->data[index + 1].r + (nx - (int) nx) / 2.0 * (modifiedPic->data[index].r - modifiedPic->data[index + 1].r);
                g_top = modifiedPic->data[index + 1].g + (nx - (int) nx) / 2.0 * (modifiedPic->data[index].g - modifiedPic->data[index + 1].g);
                b_top = modifiedPic->data[index + 1].b + (nx - (int) nx) / 2.0 * (modifiedPic->data[index].b - modifiedPic->data[index + 1].b);
                alt->data[j].r = (unsigned char) CLAMP(r_top);
                alt->data[j].g = (unsigned char) CLAMP(g_top);
                alt->data[j].b = (unsigned char) CLAMP(b_top);
                continue;
            }
            if (col == col_bord) {
                int index = w * row + col;
                int index2 = w * (row_bord) + col;
                r_top = modifiedPic->data[index].r + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].r - modifiedPic->data[index].r);
                g_top = modifiedPic->data[index].g + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].g - modifiedPic->data[index].g);
                b_top = modifiedPic->data[index].b + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].b - modifiedPic->data[index].b);
                alt->data[j].r = (unsigned char) CLAMP(r_top);
                alt->data[j].g = (unsigned char) CLAMP(g_top);
                alt->data[j].b = (unsigned char) CLAMP(b_top);
                continue;
            }
            double x_dist = fabs(round(nx) - nx);
            double y_dist = fabs(round(ny) - ny);
            r_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].r + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].r + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].r + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].r;
            g_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].g + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].g + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].g + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].g;
            b_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].b + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].b + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].b + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].b;
            alt->data[j].r = (unsigned char) CLAMP(r_top);
            alt->data[j].g = (unsigned char) CLAMP(g_top);
            alt->data[j].b = (unsigned char) CLAMP(b_top);
            continue;
        }
        if (nx > 0  && ny < 0) {
            int col_bord;
            if (w - 1 > col + 1) {
                col_bord = col + 1;
            }
            else {
                col_bord = w - 1;
            }
            int row_bord;
            if (h - 1 > row + 1) {
                row_bord = row + 1;
            }
            else {
                row_bord = h - 1;
            }
            if (row == row_bord && col == col_bord) {
                alt->data[j] = modifiedPic->data[row * w + col];
                continue;
            }
            if (row == row_bord) {
                int index = w * row + col;
                r_top = modifiedPic->data[index].r + (nx - (int) nx) / 2.0 * (modifiedPic->data[index + 1].r - modifiedPic->data[index].r);
                g_top = modifiedPic->data[index].g + (nx - (int) nx) / 2.0 * (modifiedPic->data[index + 1].g - modifiedPic->data[index].g);
                b_top = modifiedPic->data[index].b + (nx - (int) nx) / 2.0 * (modifiedPic->data[index + 1].b - modifiedPic->data[index].b);
                alt->data[j].r = (unsigned char) CLAMP(r_top);
                alt->data[j].g = (unsigned char) CLAMP(g_top);
                alt->data[j].b = (unsigned char) CLAMP(b_top);
                continue;
            }
            if (col == col_bord) {
                int index = w * row + col;
                int index2 = w * (row_bord) + col;
                r_top = modifiedPic->data[index].r + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].r - modifiedPic->data[index].r);
                g_top = modifiedPic->data[index].g + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].g - modifiedPic->data[index].g);
                b_top = modifiedPic->data[index].b + (ny - (int) ny) / 2.0 * (modifiedPic->data[index2].b - modifiedPic->data[index].b);
                alt->data[j].r = (unsigned char) CLAMP(r_top);
                alt->data[j].g = (unsigned char) CLAMP(g_top);
                alt->data[j].b = (unsigned char) CLAMP(b_top);
                continue;
            }
            double x_dist = fabs(round(nx) - nx);
            double y_dist = fabs(round(ny) - ny);
            r_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].r + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].r + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].r + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].r;
            g_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].g + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].g + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].g + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].g;
            b_top = (1 - x_dist) * (1 - y_dist) * modifiedPic->data[w * row + col].b + x_dist * (1 - y_dist) * modifiedPic->data[w * row + col + 1].b + y_dist * (1 - x_dist) * modifiedPic->data[w * (row + 1) + col].b + x_dist * y_dist * modifiedPic->data[w * (row + 1) + col + 1].b;
            alt->data[j].r = (unsigned char) CLAMP(r_top);
            alt->data[j].g = (unsigned char) CLAMP(g_top);
            alt->data[j].b = (unsigned char) CLAMP(b_top);
            continue;
        }
    }
    this->w = alt->w;
    this->h = alt->h;
    img_t *temp = modifiedPic;
    temp->data = modifiedPic->data;
    modifiedPic = alt;
    modifiedPic->data = alt->data;
    free(temp->data);
    free(temp);
}
