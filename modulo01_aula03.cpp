#include <iostream>
#include <fstream> 

using namespace std;

int main () { 
    // Cria o objeto de stream
    ofstream image_out;

    image_out.open("Antonio.ppm");

    // 1. Verifica se DEU ERRO ao abrir
    if (!image_out) {
        cerr << "Erro: O arquivo não pôde ser aberto." << endl;
        return 1; // Encerra o programa se der erro
    }
        
    // 2. Se chegou aqui, o arquivo abriu. Agora escrevemos o cabeçalho.
    image_out << "P3" << endl;
    image_out << "2 2" << endl; // Largura Altura
    image_out << "255" << endl; // Valor máximo de cor

    // 3. Escreve os Pixels (R G B)
    // Pixel 1 (Vermelho)
    image_out << "255 0 0 " << endl;
    // Pixel 2 (Vermelho escuro)
    image_out << "100 0 0 " << endl;
    // Pixel 3 (Preto)
    image_out << "0 0 0 " << endl;
    // Pixel 4 (Vermelho)
    image_out << "255 0 0 " << endl;

    image_out.close();

    return 0;
}