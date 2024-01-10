var temperatureGaugeDiv = document.getElementById("temperature-gauge");
var humidityGaugeDiv = document.getElementById("humidity-gauge");
var pressureGaugeDiv = document.getElementById("pressure-gauge");
var dewPointGaugeDiv = document.getElementById("dewPoint-gauge");
var comPort;

let temperatureArray = [];
let humidityArray = [];
let pressureArray = [];
let dewPointArray = [];
let timeStampArray = [];

fetch('/calibrationValues')
    .then((response) => response.json())
    .then((jsonResponse) => {
    // Set default values in input boxes
        document.getElementById('temp-a').value = jsonResponse.tempA;
        document.getElementById('temp-b').value = jsonResponse.tempB;

        document.getElementById('humi-a').value = jsonResponse.humiA;
        document.getElementById('humi-b').value = jsonResponse.humiB;

        document.getElementById('pres-a').value = jsonResponse.presA;
        document.getElementById('pres-b').value = jsonResponse.presB;

        document.getElementById('dewp-a').value = jsonResponse.dewpA;
        document.getElementById('dewp-b').value = jsonResponse.dewpB;

        comPort = jsonResponse.comPort;
}).catch(error => {
    console.error('Error fetching default values:', error);
});

document.getElementById('calibration-input').addEventListener('submit', function (event) {
    event.preventDefault(); // Prevent default form submission behavior

    // Retrieve values from input boxes
    let tempA = document.getElementById('temp-a').value;
    let tempB = document.getElementById('temp-b').value;

    let humiA = document.getElementById('humi-a').value;
    let humiB = document.getElementById('humi-b').value;

    let presA = document.getElementById('pres-a').value;
    let presB = document.getElementById('pres-b').value;

    let dewpA = document.getElementById('dewp-a').value;
    let dewpB = document.getElementById('dewp-b').value;

        fetch('/submitValues', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            tempA: tempA,
            tempB: tempB,
            humiA: humiA,
            humiB: humiB,
            presA: presA,
            presB: presB,
            dewpA: dewpA,
            dewpB: dewpB,
            comPort: comPort
        })
    }).then(response => {
        // Handle the response from the backend if required
    }).catch(error => {
        console.error('Error submitting values:', error);
    });

});

// History Data
fetch(`/history`)
    .then((response) => response.json())
    .then((jsonResponse) => {
        temperatureArray = jsonResponse.map(entry => entry.temperature.toFixed(2));
        humidityArray = jsonResponse.map(entry => entry.humidity.toFixed(2));
        pressureArray = jsonResponse.map(entry => entry.pressure.toFixed(2));
        dewPointArray = jsonResponse.map(entry => entry.dewPoint.toFixed(2));
        timeStampArray = jsonResponse.map(entry => entry.timeStamp);

        var temperatureTrace = {
            x: timeStampArray,
            y: temperatureArray,
            name: "Temperature",
            mode: "lines+markers",
            type: "scatter",
        };
        var humidityTrace = {
            x: timeStampArray,
            y: humidityArray,
            name: "Humidity",
            mode: "lines+markers",
            type: "scatter",
        };
        var pressureTrace = {
            x: timeStampArray,
            y: pressureArray,
            name: "Pressure",
            mode: "lines+markers",
            type: "scatter",
        };
        var dewPointTrace = {
            x: timeStampArray,
            y: dewPointArray,
            name: "Dew Point",
            mode: "lines+markers",
            type: "scatter",
        };

        var temperatureLayout = {
            autosize: true,
            title: {
                text: "Temperature",
            },
            font: {
                size: 14,
                color: "#7f7f7f",
            },
            colorway: ["#B22222"],
            height: 330,
            margin: { t: 40, b: 80, pad: 5 },
        };
        var humidityLayout = {
            autosize: true,
            title: {
                text: "Humidity",
            },
            font: {
                size: 14,
                color: "#7f7f7f",
            },
            colorway: ["#00008B"],
            height: 330,
            margin: { t: 40, b: 80, pad: 5 },
        };
        var pressureLayout = {
            autosize: true,
            title: {
                text: "Pressure",
            },
            font: {
                size: 14,
                color: "#7f7f7f",
            },
            colorway: ["#FF4500"],
            height: 330,
            margin: { t: 40, b: 80, pad: 5 },
        };
        var dewPointLayout = {
            autosize: true,
            title: {
                text: "Dew Point",
            },
            font: {
                size: 14,
                color: "#7f7f7f",
            },
            colorway: ["#008080"],
            height: 330,
            margin: { t: 40, b: 80, pad: 5 },
        };
    });


function findClosestReading() {
    const currentTime = new Date();
    let referenceTime = new Date(currentTime.getTime() - (1 * 60 * 60 * 1000));

    const targetDate = new Date(referenceTime);
    let closestIndex = 0;
    let closestDiff = Math.abs(targetDate - new Date(timeStampArray[0]));

    for (let i = 1; i < timeStampArray.length; i++) {
        const currentDiff = Math.abs(targetDate - new Date(timeStampArray[i]));
        if (currentDiff < closestDiff) {
            closestDiff = currentDiff;
            closestIndex = i;
        }
    }

    console.error('Error submitting values:', closestIndex);
    return closestIndex;
}

var temperatureData = [
    {
        domain: { x: [0, 1], y: [0, 1] },
        value: 0,
        title: { text: "Temperature" },
        type: "indicator",
        mode: "gauge+number+delta",
        delta: { reference: 0 },
        gauge: {
            axis: { range: [-10, 40] },
            steps: [
                { range: [10, 30], color: "lightgray" },
                { range: [18, 25], color: "gray" },
            ],
        },
    },
];

var humidityData = [
    {
        domain: { x: [0, 1], y: [0, 1] },
        value: 0,
        title: { text: "Humidity" },
        type: "indicator",
        mode: "gauge+number+delta",
        delta: { reference: 0 },
        gauge: {
            axis: { range: [null, 100] },
            steps: [
                { range: [20, 80], color: "lightgray" },
                { range: [40, 60], color: "gray" },
            ],
        },
    },
];

var pressureData = [
    {
        domain: { x: [0, 1], y: [0, 1] },
        value: 0,
        title: { text: "Pressure" },
        type: "indicator",
        mode: "gauge+number+delta",
        delta: { reference: 0 },
        gauge: {
            axis: { range: [900, 1100] },
            steps: [
                { range: [950, 1050], color: "lightgray" },
                { range: [980, 1020], color: "gray" },
            ],
        },
    },
];

var dewPointData = [
    {
        domain: { x: [0, 1], y: [0, 1] },
        value: 0,
        title: { text: "Dew Point" },
        type: "indicator",
        mode: "gauge+number+delta",
        delta: { reference: 0 },
        gauge: {
            axis: { range: [-20, 30] },
            steps: [
                { range: [0, 20], color: "lightgray" },
                { range: [8, 15], color: "gray" },
            ],
        },
    },
];

var layout = { width: 200, height: 180, margin: { t: 25, b: 0, l: 0, r: 0 } };

Plotly.newPlot(temperatureGaugeDiv, temperatureData, layout);
Plotly.newPlot(humidityGaugeDiv, humidityData, layout);
Plotly.newPlot(pressureGaugeDiv, pressureData, layout);
Plotly.newPlot(dewPointGaugeDiv, dewPointData, layout);

let MAX_GRAPH_POINTS = 65536;
let ctr = 0;

function updateSensorReadings() {
    fetch(`/sensorReadings`)
        .then((response) => response.json())
        .then((jsonResponse) => {
            let temperature = jsonResponse.temperature.toFixed(2);
            let humidity = jsonResponse.humidity.toFixed(2);
            let pressure = jsonResponse.pressure.toFixed(2);
            let dewPoint = jsonResponse.dewPoint.toFixed(2);
            let timeStamp = jsonResponse.timeStamp;

            updateBoxes(temperature, humidity, pressure, dewPoint);

            updateGauge(temperature, humidity, pressure, dewPoint);

        });
}

function updateBoxes(temperature, humidity, pressure, dewPoint) {
    let temperatureDiv = document.getElementById("temperature");
    let humidityDiv = document.getElementById("humidity");
    let pressureDiv = document.getElementById("pressure");
    let dewPointDiv = document.getElementById("dewPoint");

    temperatureDiv.innerHTML = temperature + " *C";
    humidityDiv.innerHTML = humidity + " %";
    pressureDiv.innerHTML = pressure + " hPa";
    dewPointDiv.innerHTML = dewPoint + " *C";
}

function updateGauge(temperature, humidity, pressure, dewPoint) {
    const referenceReadings = findClosestReading();
    var temperature_update = {
        value: temperature,
        delta: { reference: temperatureArray[referenceReadings] },

    };
    var humidity_update = {
        value: humidity,
        delta: { reference: humidityArray[referenceReadings] },
    };
    var pressure_update = {
        value: pressure,
        delta: { reference: pressureArray[referenceReadings] },
    };
    var dewPoint_update = {
        value: dewPoint,
        delta: { reference: dewPointArray[referenceReadings] },
    };
    Plotly.update(temperatureGaugeDiv, temperature_update);
    Plotly.update(humidityGaugeDiv, humidity_update);
    Plotly.update(pressureGaugeDiv, pressure_update);
    Plotly.update(dewPointGaugeDiv, dewPoint_update);
}


(function loop() {
    setTimeout(() => {
        updateSensorReadings();
        loop();
    }, 3000);
})();
