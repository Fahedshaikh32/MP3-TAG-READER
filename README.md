# 🎵 MP3 Tag Reader & Editor

This is a command-line based application written in C to read and edit MP3 file metadata (ID3 tags). The project demonstrates file handling and binary data processing concepts.

---

## 📌 Features
- View MP3 tag information (Title, Artist, Album, Year, etc.)
- Edit metadata fields easily
- Command-line based usage
- Lightweight and efficient

---

## 🛠️ Technologies Used
- C Programming
- File Handling
- Structures
- Command Line Arguments

---

## 🚀 How to Run

### Compile the program:
```

gcc *.c

```

### View MP3 tag details:
```

./a.out -v filename.mp3

```

### Edit MP3 tags:
```

./a.out -e -t "New Title" filename.mp3
./a.out -e -a "New Artist" filename.mp3
./a.out -e -A "New Album" filename.mp3
./a.out -e -y 2024 filename.mp3
./a.out -e -c "New Comment" filename.mp3

```

---

## 📖 Project Description
This project allows users to read and modify MP3 metadata using C. It works by accessing and updating ID3 tags in binary format. The implementation focuses on understanding low-level file operations and data manipulation.

---

## 📁 Project Structure
- main_view.c  
- view.c / view.h  
- edit.c / edit.h  
- common.h  

---

## 👨‍💻 Author
**Fahed Shaikh**

---

## ⭐ Note
This is a learning project developed to understand file handling and metadata processing in C.
```
