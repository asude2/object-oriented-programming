#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

class Course {
private:
    string dersAd;
    string dersNo;
    int krediSaat;
    char dersNot;
public:
    Course(const string& ad = "", const string& no = "", int kredi = 0, char nott = ' '): dersAd(ad), dersNo(no), krediSaat(kredi), dersNot(nott) {}
    void setCourseDetails(const string& ad, const string& no, int kredi, char nott);
    int getCredits() const;
    double getGradePoints() const;
    string getCourseDetails() const;
};

class personType {
protected:
    string ogrenciAd;
    string ogrenciSoyad;
public:
    personType(const string& ogrAd = "", const string& ogrSoyad = "") {
    ogrenciAd = ogrAd;
    ogrenciSoyad = ogrSoyad;
}
    string getTamAd() const;
};

class studentType : public personType {
private:
    int ogrenciKimlik;
    int dersSayisi;
    char odeme;
    Course dersler[10];
public:
    studentType() : personType(), ogrenciKimlik(0), dersSayisi(0), odeme(' ') {}
    void readFromStream(ifstream& inputFile, ofstream& outputFile);
    void printOgrenciInfo(ofstream& outputFile, int krediUcreti);
};


void Course::setCourseDetails(const string& ad, const string& no, int kredi, char nott) {
    dersAd = ad;
    dersNo = no;
    krediSaat = kredi;
    dersNot = nott;
}

int Course::getCredits() const {
    return krediSaat;
}

double Course::getGradePoints() const {
    switch (dersNot) {
    case 'A': return 4 * krediSaat;
    case 'B': return 3 * krediSaat;
    case 'C': return 2 * krediSaat;
    case 'D': return 1 * krediSaat;
    case 'F': return 0 * krediSaat;
    default: return 0;
    }
}

string Course::getCourseDetails() const {
    stringstream ss;
    ss << left << setw(12) << dersNo
        << setw(15) << dersAd
        << setw(10) << krediSaat
        << dersNot;
    return ss.str();
}

string personType::getTamAd() const {
    return ogrenciAd + " " + ogrenciSoyad;
}

void studentType::readFromStream(ifstream& inputFile, ofstream& outputFile) {
    string line;
    int ogrenciSayisi, krediUcreti;

    getline(inputFile, line);
    istringstream ss(line);
    ss >> ogrenciSayisi >> krediUcreti;

    while (getline(inputFile, line)) {
        if (line.empty() || line == "------------------------------") continue;

        if (line.find('|') != string::npos) {
            istringstream ss(line);
            string adSoyad, idStr, odemeStr, dersSayisiStr;
            getline(ss, adSoyad, '|');
            getline(ss, idStr, '|');
            getline(ss, odemeStr, '|');
            getline(ss, dersSayisiStr, '|');

            stringstream adSoyadStream(adSoyad);
            getline(adSoyadStream, ogrenciAd, ' ');
            getline(adSoyadStream, ogrenciSoyad); 

            ogrenciKimlik = stoi(idStr);
            dersSayisi = stoi(dersSayisiStr);
            odeme = odemeStr[1]; 

            for (int i = 0; i < dersSayisi; ++i) {
                string courseLine;
                if (getline(inputFile, courseLine)) {
                    istringstream courseStream(courseLine);    //courseline içindeki verileri parçalamak için kullanık.
                    string courseName, courseCode;
                    int credits;
                    char grade;

                    courseStream >> courseName >> courseCode >> credits >> grade;
                    dersler[i].setCourseDetails(courseName, courseCode, credits, grade);
                }
            }

            printOgrenciInfo(outputFile, krediUcreti);
        }
    }
}

void studentType::printOgrenciInfo(ofstream& outputFile, int krediUcreti) {

    outputFile << "Student Name: " << getTamAd() << endl;
    outputFile << "Student ID: " << ogrenciKimlik << endl;
    outputFile << "Number of courses enrolled: " << dersSayisi << endl;

    int totalCredits = 0;
    double totalGradePoints = 0;

    if (odeme == 'Y') {
        outputFile << "Course No  Course Name  Credits    Grade" << endl;
        for (int i = 0; i < dersSayisi; ++i) {
            outputFile << dersler[i].getCourseDetails() << endl;
            totalCredits += dersler[i].getCredits();
            totalGradePoints += dersler[i].getGradePoints();
        }

        double gpa = totalCredits > 0 ? totalGradePoints / totalCredits : 0.0;
        outputFile << "Total Number of Credits: " << totalCredits << endl;
        outputFile << "Mid-Semester GPA: " << fixed << setprecision(2) << gpa << endl;
        outputFile << "------------------------------" << endl;
    }
    else {
        for (int i = 0; i < dersSayisi; ++i) {
            totalCredits += dersler[i].getCredits();
        }
        int faturaTutar = krediUcreti * totalCredits;
        outputFile << "Odeme yapilmadigi icin notlar yazilmadi.\n";
        outputFile << "Odenmesi gereken tutar: " << faturaTutar << " TL" << endl;
        outputFile << "------------------------------" << endl;
    }
}


int main() {
    ifstream inputFile("girdi.txt");
    ofstream outputFile("output.txt");

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cout << "Dosya Acilamadi!" << endl;
        return 1;
    }

    studentType student;
    student.readFromStream(inputFile, outputFile);

    inputFile.close();
    outputFile.close();

    cout << "\n" << "Ogrenci bilgileri output.txt dosyasina yazildi." << "\n" << endl;

}

