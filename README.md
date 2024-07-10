# Veda - Web based biometric attendance system with database storage

## Overview
Veda is a web-based attendance system designed to address the inefficiencies of manual attendance recording in Nigerian schools and universities. Leveraging microcontroller technology and biometric verification, Veda offers a secure, accessible, and timely solution for managing student attendance. The system is developed around key components such as the Arduino UNO microcontroller, ESP8266 ESP01s WiFi module, R307S fingerprint module, and a web server, ensuring a scalable and extendable platform for various educational and professional settings.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

---


## Features

- **Student Registration:** Easily register and track student data.
- **Attendance Tracking:** Automated attendance system for accurate tracking.
- **Grade Management:** Input and calculate student grades with ease.
- **Reporting:** Generate detailed scored for students attendance performance.
- **Biometric Verification**: Utilizes R307S fingerprint module for accurate student identification.
- **Real-Time Attendance Tracking**: Marks attendance for registered students and provides immediate access to records.
- **Web-Based Access**: Attendance records are accessible through a local web server, facilitating easy management and review.
- **Scalable Design**: Can be extended to other classes, departments, and colleges with minimal changes to hardware.
- **Hardware Integration**: Incorporates Arduino UNO, ESP8266 WiFi module, and other components for a comprehensive attendance solution.

## Components
- Arduino UNO Microcontroller
- ESP8266 ESP01s WiFi Module
- R307S Fingerprint Module
- 1602a LCD Display
- LEDs, Push Buttons
- I2C Adapter Module

## Installation
1. **Hardware Setup**: Assemble the hardware components based on the circuit diagrams provided in the appendices.
2. **Software Configuration**: Upload the Arduino and server codes to the microcontroller and configure the local web server.
3. **System Testing**: Follow the testing procedures outlined in the documentation to ensure system integrity and performance.

### Server Deployment
### Server Deployment

To deploy the Veda server, follow these steps:

1. Clone the repository to your local machine:
    ```bash
    git clone https://github.com/Jesulayomi/Veda.git
    ```

2. Navigate to the project directory:
    ```bash
    cd Veda
    ```

3. Create a virtual environment and activate it:
    ```bash
    python3 -m venv venv
    source venv/bin/activate
    ```

4. Install the required dependencies:
    ```bash
    pip install -r requirements.txt
    ```

5. Set up the environment variables:
    - Create a `.env` file in the project root directory.
    - Add the following variables to the `.env` file:
    ```env
    DB_VEDA_MODE = MODE
    DB_VEDA_HOST = HOST
    DB_VEDA_USER = USER
    DB_VEDA_PASSWORD = PWD
    DB_VEDA_NAME = NAME
    SECRET_KEY = KEY
    VEDA_HOST = HOST
    VEDA_PORT = PORT
    ```

6. Run database creation script:
    ```bash
    cat reg.sql | mysql -u root -p
    ```

7. Start the server with flask:
    ```bash
    cd api/
    flask run
    ```

    Access the server at `http://localhost:5000/` and make sure everything is working correctly.

8. GitHub Actions will run the tests on the server, or fork the [Postman Collection]()


9. Configure Nginx for deployment:
    - Install Nginx on your server if it's not already installed.
    - Create an Nginx configuration file for your Veda server. For example, create a file named `veda.conf` in the `/etc/nginx/conf.d/` or `/etc/nginx/sites-available/` directory.
    - Add the following configuration to the `veda.conf` file:
      ```
      server {
            listen 80;
            server_name your_domain;

            location / {
                proxy_pass http://localhost:5000;
                proxy_set_header Host $host;
                proxy_set_header X-Real-IP $remote_addr;
            }
      }
      ```
    - Save the file and exit.

10. Restart Nginx to apply the changes:
     ```
     sudo service nginx restart
     ```

11. Access your Veda server using your domain name or server IP address.

Congratulations! You have successfully deployed the Veda server using Nginx.


## Usage

- **Registering a Student:**
  Click the enroll button on the device and place student finger on the sensor to register. An ID will be assigned to the student.
- **Updating Student Information:**
  On the web interface, click on edit to edit a student's information. First name, last name, and matric number can be updated.
- **Marking Attendance:**
  Click the attend button on the device and place the student's finger on the sensor. The student's attendance will be recorded, and sent to the web application's server, the database will be updated and the student will be marked present.
- **Access Records**: View and download attendance records with scores through the web-based interface.

---
## Acknowledgements

This project was developed by Aina Jesulayomi Michael, under the guidance of Dr. O. I. Adebisi, and in partial fulfillment of the requirements for the Bachelor of Engineering degree at the Federal University of Agriculture, Abeokuta. Special thanks to all who supported and contributed to the success of this project.

## Contributing

We welcome contributions to Veda! If you have any ideas, suggestions, or bug reports, please open an issue or submit a pull request. Follow these steps to contribute:

1. **Fork the Repository:**
   Click the 'Fork' button at the top right of the repository page.
2. **Clone Your Fork:**
   ```bash
   git clone https://github.com/your-username/Veda.git
   ```
3. **Create a Branch:**
   ```bash
   git checkout -b feature-name
   ```
4. **Make Your Changes and Commit:**
   ```bash
   git commit -m "Description of your changes"
   ```
5. **Push to Your Fork:**
   ```bash
   git push origin feature-name
   ```
6. **Open a Pull Request:**
   Go to the original repository and open a pull request with a detailed description of your changes.

## License
Veda is licensed under the MIT License. See the LICENSE file for more details.


## Contact
For any inquiries or support, please contact:

- **Owner:** Jesulayomi Michael Aina
- **LinkedIn:** [Jesulayomi Aina](https://linkedin.com/in/jesulayomi)
- **GitHub:** [Jesulayomy](https://github.com/Jesulayomy)

---

Feel free to explore and contribute to the Veda project. Your feedback and participation are highly valued!
