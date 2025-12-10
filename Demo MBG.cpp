#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <iomanip>

using namespace std;

// ---------------------- STEMMER SEDERHANA INDONESIA ---------------------
string stemWord(string w) {
    if (w.size() > 3 && w.compare(w.size() - 3, 3, "nya") == 0) {
        w = w.substr(0, w.size() - 3);
    }

    if (w == "nggak") return "tidak";
    if (w == "gak" || w == "ga") return "tidak";

    return w;
}

string normalizeBigram(string a, string b) {
    string combined = a + " " + b;

    if (combined == "nggak enak" || combined == "gak enak" || combined == "ga enak")
        return "tidak enak";

    if (combined == "kurang sedap")
        return "kurang enak";

    return combined;
}

bool inVector(const vector<string>& v, const string& s) {
    return find(v.begin(), v.end(), s) != v.end();
}

int main() {
    int n;
    cout << "Jumlah feedback: ";
    cin >> n;
    cin.ignore();

    // ------------------- KEYWORDS DENGAN KATEGORI --------------------
    map<string, string> category;

    vector<string> rasa = {"enak", "tidak enak", "kurang enak", "rasa"};
    vector<string> porsi = {"sedikit", "banyak"};
    vector<string> aroma = {"bau", "tidak bau"};
    vector<string> rasa_spesifik = {
        "asin", "manis", "pedas", "asam", "hangat", "dingin", "pahit", "gurih"
    };
    vector<string> kondisi = {
        "tidak layak", "beracun", "segar", "basi", "mentah", "matang"
    };

    vector<string> perhatian_khusus = {
        "alergi", "alergen", "mengandung kacang",
        "alergi susu", "alergi telur", "alergi seafood",
        "alergi ikan", "alergi kerang", "alergi kacang",
        "alergi gandum", "alergi tepung"
    };

    for (auto &x : rasa) category[x] = "Rasa";
    for (auto &x : porsi) category[x] = "Porsi";
    for (auto &x : aroma) category[x] = "Aroma";
    for (auto &x : rasa_spesifik) category[x] = "Rasa Spesifik";
    for (auto &x : kondisi) category[x] = "Kondisi Makanan";
    for (auto &x : perhatian_khusus) category[x] = "Perhatian Khusus";

    vector<string> keywords;
    for (auto &p : category) keywords.push_back(p.first);

    // ------------------- SENTIMEN POSITIF / NEGATIF --------------------
    map<string, string> sentiment;

    vector<string> positif = {"enak", "tidak bau", "segar", "matang"};

    vector<string> negatif = {
        "tidak enak", "kurang enak", "tidak layak", "beracun",
        "basi", "mentah",
        "alergi", "alergen", "mengandung kacang",
        "alergi susu", "alergi telur", "alergi seafood",
        "alergi ikan", "alergi kerang", "alergi kacang",
        "alergi gandum", "alergi tepung"
    };

    for (auto &x : positif) sentiment[x] = "Positif";
    for (auto &x : negatif) sentiment[x] = "Negatif";

    // ----------------- INPUT FEEDBACK -----------------
    vector<string> feedbacks;
    string text;

    for (int i = 0; i < n; i++) {
        cout << "Feedback " << i + 1 << ": ";
        getline(cin, text);
        feedbacks.push_back(text);
    }

    map<string, int> freq;
    int totalPositif = 0;
    int totalNegatif = 0;

    // ----------------- PROSES FEEDBACK ----------------
    for (string fb : feedbacks) {
        stringstream ss(fb);
        vector<string> words;
        string w;

        while (ss >> w) {
            for (int i = 0; i < w.size(); i++) {
                if (ispunct(w[i])) {
                    w.erase(i, 1);
                    i--;
                }
            }

            for (char &c : w) c = tolower(c);

            w = stemWord(w);

            if (w != "") words.push_back(w);
        }

        // BIGRAM
        for (int i = 0; i + 1 < words.size(); i++) {
            string normalized = normalizeBigram(words[i], words[i+1]);

            if (inVector(keywords, normalized)) {
                freq[normalized]++;

                if (sentiment.count(normalized)) {
                    if (sentiment[normalized] == "Positif") totalPositif++;
                    else totalNegatif++;
                }

                words[i] = "";
                words[i+1] = "";
            }
        }

        // UNIGRAM
        for (string &x : words) {
            if (x != "" && inVector(keywords, x)) {
                freq[x]++;

                if (sentiment.count(x)) {
                    if (sentiment[x] == "Positif") totalPositif++;
                    else totalNegatif++;
                }
            }
        }
    }

    // ------------------- OUTPUT TABEL -------------------------
    cout << "\n=====================================================\n";
    cout << "     TABEL KATA KUNCI FEEDBACK MAKANAN\n";
    cout << "=====================================================\n";

    cout << left << setw(22) << "Kata Kunci"
         << setw(20) << "Kategori"
         << setw(15) << "Sentimen"
         << setw(10) << "Jumlah" << endl;

    cout << "---------------------------------------------------------------\n";

    vector<pair<string,int>> sorted;
    for (auto &p : freq) sorted.push_back(p);

    sort(sorted.begin(), sorted.end(),
        [](auto &a, auto &b) { return a.second > b.second; });

    for (auto &p : sorted) {
        cout << left << setw(22) << p.first
             << setw(20) << category[p.first]
             << setw(15) << sentiment[p.first]
             << setw(10) << p.second
             << endl;
    }

    cout << "---------------------------------------------------------------\n";

    // ------------------- RINGKASAN SENTIMEN -------------------------
    cout << "\n=============== RINGKASAN SENTIMEN ===============\n";
    cout << left << setw(20) << "Sentimen"
         << setw(10) << "Total" << endl;
    cout << "---------------------------------------------\n";
    cout << left << setw(20) << "Positif" << totalPositif << endl;
    cout << left << setw(20) << "Negatif" << totalNegatif << endl;
    cout << "---------------------------------------------\n";

    cout << "=====================================================\n";

    return 0;
}
