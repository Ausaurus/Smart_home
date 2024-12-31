const express = require('express');
const bodyParser = require('body-parser');

const app = express();
const port = 3000;

app.use((req, res, next) => {
    res.setHeader('Access-Control-Allow-Origin', '*'); // Allow all origins
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
    next();
});

app.use(bodyParser.json()); // Parse JSON bodies

let sensorData = { temperature: 0.0, light: 0.0 };

// Endpoint to receive data from ESP32
app.post('/update', (req, res) => {
    sensorData = req.body; // Capture data sent by ESP32
    console.log('Received Data:', sensorData);
    res.status(200).send('Data received');
});

// Endpoint to send data to a client
app.get('/data', (req, res) => {
    res.json(sensorData);
});

app.listen(port, () => {
    console.log(`Server running on http://localhost:${port}`);
});
