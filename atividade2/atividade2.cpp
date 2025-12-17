#include<iostream>
#include<fstream>

using namespace std;

int main() {
    ofstream image;

    image.open("./atividade2.ppm");

    if (image.is_open()) {
        image << "P3" << endl;
        image << "100 100" << endl;
        image << "255" << endl;

        for (int ImgH = 0; ImgH < 100; ImgH++) {
            for (int ImhV = 0; ImhV < 100; ImhV++) {
                image << ImgH * 10 << " " << "0" << " " << ImhV << endl;
            }
        }
    }

    image.close();

    return 0;
}