#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <thread>
#include <msclr/marshal_cppstd.h>
#include "../include/stb_image.h"
#include "../include/Filters.h"
#include "../include/Filters_Cuda.h"

namespace PavicGui2024 {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using namespace System::Drawing::Imaging;

    public ref class MyForm : public System::Windows::Forms::Form
    {
    public:
        MyForm(void)
        {
            InitializeComponent();
            pixelData = nullptr; 
        }

    protected:
        ~MyForm()
        {
            if (components)
            {
                delete components;
            }
            if (pixelData) stbi_image_free(pixelData);
        }

    private:
        unsigned char* pixelData;
        int imgWidth, imgHeight, imgChannels;

        System::Windows::Forms::PictureBox^ pictureBoxOriginal;
        System::Windows::Forms::PictureBox^ pictureBoxResult;
        System::Windows::Forms::Label^ lblTimeSingle;
        System::Windows::Forms::Label^ lblTimeMulti;
        System::Windows::Forms::Label^ lblTimeCuda;

        void UpdateDisplay() {
            if (pixelData == nullptr) return;

            System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(imgWidth, imgHeight, imgWidth * 3, 
                System::Drawing::Imaging::PixelFormat::Format24bppRgb, (System::IntPtr)pixelData);
            
            pictureBoxResult->Image = bmp;
        }

        private: System::Void btnLoad_Click(System::Object^ sender, System::EventArgs^ e) {
            System::Windows::Forms::OpenFileDialog^ ofd = gcnew System::Windows::Forms::OpenFileDialog();
            ofd->Filter = "Imagens|*.jpg;*.png;*.bmp;*.ppm";

            if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                std::string path = msclr::interop::marshal_as<std::string>(ofd->FileName);

                if (pixelData) stbi_image_free(pixelData);

                pixelData = stbi_load(path.c_str(), &imgWidth, &imgHeight, &imgChannels, 3);

                if (pixelData) {
                    pictureBoxOriginal->Image = System::Drawing::Image::FromFile(ofd->FileName);
                    UpdateDisplay();
                }
            }
        }

        private: System::Void btnSepiaMulti_Click(System::Object^ sender, System::EventArgs^ e) {
            if (!pixelData) return;
            auto start = std::chrono::high_resolution_clock::now();

            int numThreads = std::thread::hardware_concurrency();
            std::vector<std::thread> threads;
            int sliceHeight = imgHeight / numThreads;

            for (int i = 0; i < numThreads; i++) {
                int startY = i * sliceHeight;
                int endY = (i == numThreads - 1) ? imgHeight : (i + 1) * sliceHeight;
                threads.push_back(std::thread(applySepiaCPU, pixelData, startY, endY, imgWidth, 3));
            }

            for (auto& t : threads) t.join();

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> ms = end - start;
            lblTimeMulti->Text = ms.count().ToString("F2") + " ms";
            UpdateDisplay();
        }

        private: System::Void btnSepiaCuda_Click(System::Object^ sender, System::EventArgs^ e) {
            if (!pixelData) return;
            auto start = std::chrono::high_resolution_clock::now();

            launchSepiaCUDA(pixelData, imgWidth, imgHeight, 3);

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> ms = end - start;
            lblTimeCuda->Text = ms.count().ToString("F2") + " ms";
            UpdateDisplay();
        }

    private:
        System::ComponentModel::IContainer^ components;

#pragma region Windows Form Designer generated code
        void InitializeComponent(void)
        {
            this->pictureBoxOriginal = (gcnew System::Windows::Forms::PictureBox());
            this->pictureBoxResult = (gcnew System::Windows::Forms::PictureBox());
            this->lblTimeSingle = (gcnew System::Windows::Forms::Label());
            this->lblTimeMulti = (gcnew System::Windows::Forms::Label());
            this->lblTimeCuda = (gcnew System::Windows::Forms::Label());
            
            this->SuspendLayout();
            this->Size = System::Drawing::Size(800, 600);
            this->Text = L"ImpactLab 2025";
            this->ResumeLayout(false);
        }
#pragma endregion
    };
}