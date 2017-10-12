# Python-GPIO-SMS
---
Python script who interract with Gammu to control GPIO on RaspberryPI 3 B, made by Kiiway.

# Use the program
---
To use this script you need to put it in execute file :
```bash
sudo chmod +x /your/folder/location/Python-GPIO-SMS/main.py
```
To run the script you need run this command:
```bash
sudo python /your/folder/location/Python-GPIO-SMS/main.py
```

> **Note:**
>Now you can send SMS to the phone number of the dongle like this :
> **gpio:[Chanel];[State(1|0)]**

# Install gammu from sources
---

Place you in the gammu folder : 

```bash
cd /your/folder/location/Python-GPIO-SMS/lib/gammu-1.38.4
```

```bash
./configure
```
Building the program
```bash
make
```
Install built program
```bash
make install
```

# Install WiringPi from sources
---

Place you in the WiringPi folder : 

```bash
cd your/folder/location/Python-GPIO-SMS/lib/WiringPi-master
```

Build and install the WiringPI
```bash
./build
```

# Install gammu for python
---

```bash
sudo python /your/folder/location/Python-GPIO-SMS/lib/python-gammu-2.9/setup.py
```

# Install WiringPI for python
---

```bash
sudo python /your/folder/location/Python-GPIO-SMS/lib/RPi.GPIO-0.6.3/setup.py
```