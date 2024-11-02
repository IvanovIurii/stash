#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>

#define KEY "key"
#define SIZE 4
#define ENCRYPTED_CHAR 0x45

using namespace std;
namespace fs = std::filesystem;

char *symmetricHash(const char *input, const int size, const string key);

int encrypt(fstream &inputFile, const string key);
int decrypt(fstream &inputFile, const string key);
bool isEncrypted(fstream &inputFile);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cout << "Invalid amount of arguments!" << endl;
        return 1;
    }

    char *fileName = argv[2];
    if (!fs::exists(fileName))
    {
        cout << "File '" << fileName << "' does not exist!" << endl;
        return 1;
    }

    fstream inputFile(fileName, ios::binary | ios_base::in | ios_base::out);

    char *command = argv[1];
    if ("--encrypt" == string(command))
    {
        return encrypt(inputFile, KEY);
    }
    if ("--decrypt" == string(command))
    {
        return decrypt(inputFile, KEY);
    }

    inputFile.close();
    return 0;
}

char *symmetricHash(const char *input, const int size, const string key)
{
    char *result;
    strncpy(result, input, size);

    for (int i = 0; i < size; ++i)
    {
        result[i] = input[i] ^ key[i % key.length()];
    }
    return result;
}

void toggle(fstream &inputFile, const string key)
{
    inputFile.seekp(0, ios_base::beg);

    char buffer[SIZE];
    inputFile.read(buffer, SIZE);

    char *hashedBuffer = symmetricHash(buffer, SIZE, KEY);
    inputFile.seekp(0, ios_base::beg);
    inputFile.write(hashedBuffer, SIZE);
}

int encrypt(fstream &inputFile, const string key)
{
    if (isEncrypted(inputFile))
    {
        cout << "Already encrypted!" << endl;
        return 1;
    }

    toggle(inputFile, key);

    char c = ENCRYPTED_CHAR;
    inputFile.seekp(SIZE, ios_base::end);
    inputFile.write(&c, 1);

    return 0;
}

int decrypt(fstream &inputFile, const string key)
{
    if (!isEncrypted(inputFile))
    {
        cout << "Not encrypted!" << endl;
        return 1;
    }

    toggle(inputFile, key);

    return 0;
}

bool isEncrypted(fstream &inputFile)
{
    inputFile.seekp(-1, ios::end);

    char lastChar;
    inputFile.read(&lastChar, 1);

    if ((0xFF & lastChar) == ENCRYPTED_CHAR)
    {
        return true;
    }
    return false;
}
