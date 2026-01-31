#include "logica.h"
#include "const.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <limits>

// review - 
char processLetter(char letter, bool reverse) {
    if (!isalpha(letter)) return letter;
    letter = toupper(letter);

    //review
    if (!reverse) {
        int pos = (letter - 'A' + rotor1.position) % 26;
        letter = rotor1.wiring[pos];
        
        pos = (letter - 'A' + rotor2.position) % 26;
        letter = rotor2.wiring[pos];

        pos = (letter - 'A' + rotor3.position) % 26;
        letter = rotor3.wiring[pos];
    }
    else {
        int pos = rotor3.wiring.find(letter);
        pos = (pos - rotor3.position + 26) % 26;
        letter = 'A' + pos;

        pos = rotor2.wiring.find(letter);
        pos = (pos - rotor2.position + 26) % 26;
        letter = 'A' + pos;

        pos = rotor1.wiring.find(letter);
        pos = (pos - rotor1.position + 26) % 26;
        letter = 'A' + pos;
    }

    return letter;
}

//review
// Obtenim el missatge de l'usuari
std::string getMessageFromUser() {
    std::cout << "Introduce el mensaje: ";
    std::string message;
    getline(std::cin, message);
    return message;
}

//review
// Xifrat/desxifrat d'arxius
void encryptDecryptFile(bool encrypt) {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Posiciones iniciales (ej. A B C): ";
    std::string windowSettings;
    getline(std::cin, windowSettings);

    std::istringstream iss(windowSettings);
    char c1, c2, c3;
    if (iss >> c1 >> c2 >> c3) {
        rotor1.position = toupper(c1) - 'A';
        rotor2.position = toupper(c2) - 'A';
        rotor3.position = toupper(c3) - 'A';
    }
//review - 77
    std::string message = encrypt ? getMessageFromUser() : [&]() {
        std::ifstream inputFile(ENCRYPTED_FILE);
        std::string content;
        if (inputFile) {
            content.assign((std::istreambuf_iterator<char>(inputFile)),
                std::istreambuf_iterator<char>());
        }
        return content;
        }();

    std::string cleaned = cleanMessage(message);
    std::string result;

    for (char c : cleaned) {
        result += processLetter(c, !encrypt);
        rotateRotors();
    }

    groupText(result);

    std::string outputFilename = encrypt ? ENCRYPTED_FILE : DECRYPTED_FILE;
    std::ofstream outputFile(outputFilename);
    if (outputFile) {
        outputFile << result;
    }

    std::cout << (encrypt ? "Cifrado" : "Descifrado") << " completado. "
        << "Resultado en " << outputFilename << std::endl;
}

// Editar/modificar la configuració dels rotors
void editRotor() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Selecciona rotor (1-3): ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    if (choice < 1 || choice > 3) return;
    //review 110
    Rotor* rotor = (choice == 1) ? &rotor1 : (choice == 2) ? &rotor2 : &rotor3;
    std::string filename = "Rotor" + std::to_string(choice) + ".txt";

    std::cout << "Nuevo cableado (26 letras únicas): ";
    std::string newWiring;
    getline(std::cin, newWiring);

    if (!validateWiring(newWiring)) return;

    std::cout << "Posición del notch (A-Z): ";
    char newNotch;
    std::cin >> newNotch;
    newNotch = toupper(newNotch);
    std::cin.ignore();

    rotor->wiring = newWiring;
    rotor->notch = newNotch;

    std::ofstream file(filename);
    if (file) {
        file << rotor->wiring << '\n' << rotor->notch;
    }
}

//impresio del menu per escollir
void showMenu() {
    int choice;
    do {
        std::cout << "\nENIGMA - Menu Principal\n"
            << "1. Cifrar mensaje\n"
            << "2. Descifrar mensaje\n"
            << "3. Editar rotors\n"
            << "4. Salir\n"
            << "Selecciona: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore();

        switch (choice) {
        case 1: encryptDecryptFile(true); break;
        case 2: encryptDecryptFile(false); break;
        case 3: editRotor(); break;
        case 4: std::cout << "Saliendo...\n"; break;
        default: std::cout << "Opción no válida\n";
        }
    } while (choice != 4);
}

std::string cleanMessage(const std::string& message) {
    std::string cleaned;
    for (char c : message) {
        if (isalpha(c)) {
            cleaned += toupper(c);
        }
    }
    return cleaned;
}

//funcio per separar de 5 en 5 (lletres)
void groupText(std::string& text) {
    std::string grouped;
    for (size_t i = 0; i < text.size(); ++i) {
        grouped += text[i];
        if ((i + 1) % 5 == 0 && i != text.size() - 1) {
            grouped += ' ';
        }
    }
    text = grouped;
}
