# Event Management System (C++)

This is a **C++ Event Management System** that allows staff and users to register, log in, and manage events, equipment, and crisis handling.  
Passwords are **secured with SHA-256 hashing** using OpenSSL.

---

## ⚙️ Requirements
- **C++17** or later
- **Visual Studio 2019/2022**
- [OpenSSL for Windows](https://slproweb.com/products/Win32OpenSSL.html) (needed for password hashing)

---

## 🖥️ Setting Up OpenSSL on Windows (Visual Studio)

### Step 1: Download OpenSSL
1. Go to [https://slproweb.com/products/Win32OpenSSL.html](https://slproweb.com/products/Win32OpenSSL.html)  
2. Download the latest **Win64 OpenSSL** (choose *full* installer).  
   Example: `Win64OpenSSL-3.5.2`

---

### Step 2: Install OpenSSL
- Install to `C:\OpenSSL-Win64` (recommended).
- Make sure to install the **libraries** (`lib`) and **include headers** (`include`).

---

### Step 3: Add OpenSSL to PATH
1. Press **Win + R**, type `sysdm.cpl`, press Enter.  
2. Go to **Advanced → Environment Variables**.  
3. Under *System variables*, select `Path` → **Edit** -> **New**.  
4. Add:
   C:\Program File\OpenSSL-Win64\bin (depends on the file location)
6. Click OK.

---

### Step 4: Configure Visual Studio Project
1. Open your solution (`Ass.sln`) in **Visual Studio**.  
2. Right-click your project → **Properties**.  
3. Under **Configuration Properties → C/C++ → General → Additional Include Directories** add:
  C:\Program File\OpenSSL-Win64\include (depends on the file location)
4. Under **Configuration Properties → Linker → General → Additional Library Directories** add:
  C:\Program File\OpenSSL-Win64\lib\VC\x64\MD (depends on the file location)
5. Under **Configuration Properties → Linker → Input → Additional Dependencies** add:
  libssl.lib;libcrypto.lib

---

### Step 5: Build and Run
- Rebuild your project (**Ctrl+Shift+B**).  
- Run it (**Ctrl+F5**).  

If everything is set up correctly, your project will now compile with SHA-256 hashing enabled.

---
