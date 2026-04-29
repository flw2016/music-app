#include <iostream>
#include <sqlite3.h>
#include <string>

const char* create_songs = 
    "CREATE TABLE IF NOT EXISTS songs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "title TEXT NOT NULL, "
        "artist TEXT"
    ");";

const char* create_reviews = 
    "CREATE TABLE IF NOT EXISTS song_reviews ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "song_id INTEGER NOT NULL, "
        "thought TEXT, "
        "atmosphere INTEGER, "
        "lyrics INTEGER, "
        "music INTEGER, "
        "vocals INTEGER, "
        "sound INTEGER, "
        "FOREIGN KEY (song_id) REFERENCES songs(id)"
    ");";

const char* create_relationships = 
    "CREATE TABLE IF NOT EXISTS song_relationships ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "song_id_a INTEGER NOT NULL, "
        "song_id_b INTEGER NOT NULL, "
        "relation_type TEXT, "
        "note TEXT, "
        "FOREIGN KEY (song_id_a) REFERENCES songs(id), "
        "FOREIGN KEY (song_id_b) REFERENCES songs(id)"
    ");";

enum class QueryType {
    INSERT,
    UPDATE,
    DELETE,
    SELECT
};

int print_callback(void* data, int argc, char** argv, char** colNames) {
    for (int i = 0; i < argc; i++) {
        std::cout << colNames[i] << ": " << (argv[i] ? argv[i] : "NULL") << " | ";
    }
    std::cout << std::endl;
    return 0;
}
int get_id_callback(void* data, int argc, char** argv, char** colNames) {
    if (argc > 0 && argv[0] != nullptr) {
        int* id_ptr = static_cast<int*>(data);  
        *id_ptr = std::stoi(argv[0]);          
    }
    return 0;
}

int get_id_songs(sqlite3* db, const char* artist = nullptr, const char* title = nullptr){
    int id = -1;
    std::string sql = "SELECT id FROM songs WHERE artist = '";
    sql += artist;
    sql += "' AND title = '";
    sql += title;
    sql += "';";
    sqlite3_exec(db, sql.c_str(), get_id_callback, &id, nullptr);
    return id;
}

int get_id_reviews(sqlite3* db, int song_id = -1) {
    int id = -1;
    if (song_id == -1) return -1;
    std::string sql = "SELECT id FROM song_reviews WHERE song_id = ";
    sql += std::to_string(song_id);
    sql += ";";
    sqlite3_exec(db, sql.c_str(), get_id_callback, &id, nullptr);
    return id;
}

int get_id_relationship(sqlite3* db, int song_id_a, int song_id_b) {
    int id = -1;
    if (song_id_a == -1 || song_id_b == -1) return -1;
    
    std::string sql = "SELECT id FROM song_relationships WHERE song_id_a = ";
    sql += std::to_string(song_id_a);
    sql += " AND song_id_b = ";
    sql += std::to_string(song_id_b);
    sql += ";";
    
    sqlite3_exec(db, sql.c_str(), get_id_callback, &id, nullptr);
    return id;
}

int exec_for_songs(sqlite3* db, QueryType type, int id = -1, 
                   const char* artist = nullptr, const char* title = nullptr){
    std::string sql = "";
    switch (type)
    {
    case QueryType::INSERT:
        sql = "INSERT INTO songs (artist, title) VALUES ('";
        sql += artist;
        sql += "', '";
        sql += title;
        sql += "');";
        break;
    case QueryType::UPDATE:
        if (id == -1) return -1;
        sql += "UPDATE songs SET artist = '";
        sql += artist; sql += "', title = '";
        sql += title; sql += "' WHERE id = ";
        sql += std::to_string(id); sql += ";";
        break;
    case QueryType::DELETE:
        sql += "DELETE FROM songs WHERE id = ";
        sql += std::to_string(id);
        sql += ";";
        break;
    }
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    return rc;
}

int exec_for_reviews(sqlite3* db, QueryType type, int song_id = -1, const char* thought = nullptr,
                    int atmosphere = -1, int lyrics = -1,
                    int music = -1, int vocals = -1, int sound = -1){
    std::string sql = "";
    int id = get_id_reviews(db, song_id);
    switch (type)
    {
    case QueryType::INSERT:
        sql = "INSERT INTO song_reviews (song_id, thought, atmosphere, lyrics, music, vocals, sound) VALUES (";
        sql += std::to_string(song_id); sql += ", '";
        sql += thought; sql += "', ";
        sql += std::to_string(atmosphere); sql += ", ";
        sql += std::to_string(lyrics); sql += ", ";
        sql += std::to_string(music); sql += ", ";
        sql += std::to_string(vocals); sql += ", ";
        sql += std::to_string(sound); sql += ");";
        break;
    case QueryType::UPDATE:
        if (id == -1) {
        std::cerr << "Review not found!\n";
        return -1;
        }
        sql += "UPDATE song_reviews SET thought = '"; sql += thought;
        sql += "', atmosphere = "; sql += std::to_string(atmosphere);
        sql += ", lyrics = "; sql += std::to_string(lyrics);
        sql += ", music = "; sql += std::to_string(music);
        sql += ", vocals = "; sql += std::to_string(vocals);
        sql += ", sound = "; sql += std::to_string(sound);
        sql += " WHERE id = "; sql += std::to_string(id); sql += ";";
        break;
    case QueryType::DELETE:
        sql += "DELETE FROM song_reviews WHERE id = ";
        sql += std::to_string(id);
        sql += ";";
        break;
    }
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    return rc;
}
int exec_for_relationships(sqlite3* db, QueryType type,
                           int song_id_a = -1, int song_id_b = -1,
                           const char* relation_type = nullptr,
                           const char* note = nullptr) {
    std::string sql = "";
    int id = get_id_relationship(db, song_id_a, song_id_b);
    switch (type) {
        case QueryType::INSERT:
            sql = "INSERT INTO song_relationships (song_id_a, song_id_b, relation_type, note) VALUES (";
            sql += std::to_string(song_id_a); sql += ", ";
            sql += std::to_string(song_id_b); sql += ", '";
            sql += relation_type; sql += "', '";
            sql += note; sql += "');";
            break;
        case QueryType::UPDATE:
            if (id == -1) {
                std::cerr << "Relationship not found!\n";
                return -1;
            }
            sql += "UPDATE song_relationships SET relation_type = '";
            sql += relation_type; sql += "', note = '";
            sql += note; sql += "' WHERE id = ";
            sql += std::to_string(id); sql += ";";
            break;
        case QueryType::DELETE:
            if (id == -1) {
                std::cerr << "Relationship not found for delete!\n";
                return -1;
            }
            sql += "DELETE FROM song_relationships WHERE id = ";
            sql += std::to_string(id); sql += ";";
            break;
    }
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    return rc;
}

void init_database(sqlite3* db){
    sqlite3_exec(db, create_songs, nullptr, nullptr, nullptr);
    sqlite3_exec(db, create_reviews, nullptr, nullptr, nullptr);
    sqlite3_exec(db, create_relationships, nullptr, nullptr, nullptr);
}

void add_song(sqlite3* db, const char* artist, const char* title){
    exec_for_songs(db, QueryType::INSERT, -1, artist, title);
}
void add_review(sqlite3* db, const char* artist, const char* title, 
                const char* thought = nullptr,
                int atmosphere = -1, int lyrics = -1,
                int music = -1, int vocals = -1, int sound = -1){
    int id = get_id_songs(db, artist, title);
    exec_for_reviews(db, QueryType::INSERT, id, thought, atmosphere, lyrics, music, vocals, sound);
}
void add_relationship(sqlite3* db, int song_id_a, int song_id_b,
                      const char* relation_type = nullptr,
                      const char* note = nullptr) {
    exec_for_relationships(db, QueryType::INSERT, song_id_a, song_id_b, relation_type, note);
}

void update_song(sqlite3* db, const char* artist, const char* title, const char* new_artist = nullptr, const char* new_title = nullptr){
    int id = get_id_songs(db, artist, title);
    exec_for_songs(db, QueryType::UPDATE, id, new_artist, new_title);
}
void update_review(sqlite3* db, const char* artist, const char* title, 
                const char* thought = nullptr,
                int atmosphere = -1, int lyrics = -1,
                int music = -1, int vocals = -1, int sound = -1){
    int id = get_id_songs(db, artist, title);
    exec_for_reviews(db, QueryType::UPDATE, id, thought, atmosphere, lyrics, music, vocals, sound);
}
void update_relationship(sqlite3* db, int song_id_a, int song_id_b,
                         const char* relation_type = nullptr,
                         const char* note = nullptr) {
    exec_for_relationships(db, QueryType::UPDATE, song_id_a, song_id_b, relation_type, note);
}

void delete_song(sqlite3* db, const char* artist, const char* title){
    int id = get_id_songs(db, artist, title);
    exec_for_songs(db, QueryType::DELETE, id);
}
void delete_review(sqlite3* db, const char* artist, const char* title){
    int id = get_id_songs(db, artist, title);
    exec_for_reviews(db, QueryType::DELETE, id);
}
void delete_relationship(sqlite3* db, int song_id_a, int song_id_b) {
    exec_for_relationships(db, QueryType::DELETE, song_id_a, song_id_b);
}

void print_all_songs(sqlite3* db) {
    std::cout << "\n=== SONGS ===\n";
    sqlite3_exec(db, "SELECT * FROM songs", print_callback, nullptr, nullptr);
}
void print_all_reviews(sqlite3* db) {
    std::cout << "\n=== REVIEWS ===\n";
    sqlite3_exec(db, "SELECT * FROM song_reviews", print_callback, nullptr, nullptr);
}
void print_all_relationships(sqlite3* db) {
    std::cout << "\n=== RELATIONSHIPS ===\n";
    sqlite3_exec(db, "SELECT * FROM song_relationships", print_callback, nullptr, nullptr);
}

int main() {
    sqlite3* db;
    sqlite3_open("my_db.db", &db);
    init_database(db);
    
    add_song(db, "Metallica", "Nothing Else Matters");
    add_song(db, "Metallica", "Enter Sandman");
    add_song(db, "Slipknot", "Duality");
    
    add_review(db, "Metallica", "Nothing Else Matters", "Шедевр", 10, 9, 10, 10, 10);
    
    add_relationship(db, 1, 2, "похожи", "обе от Metallica");
    
    print_all_songs(db);
    print_all_reviews(db);
    print_all_relationships(db);
    
    sqlite3_close(db);
    return 0;
}