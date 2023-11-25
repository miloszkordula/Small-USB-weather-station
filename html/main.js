var temperatureHistoryDiv = document.getElementById("temperature-history");
var humidityHistoryDiv = document.getElementById("humidity-history");
var pressureHistoryDiv = document.getElementById("pressure-history");
var dewPointHistoryDiv = document.getElementById("dewPoint-history");

var temperatureGaugeDiv = document.getElementById("temperature-gauge");
var humidityGaugeDiv = document.getElementById("humidity-gauge");
var pressureGaugeDiv = document.getElementById("pressure-gauge");
var dewPointGaugeDiv = document.getElementById("dewPoint-gauge");

// History Data
var temperatureTrace = {
  x: [],
  y: [],
  name: "Temperature",
  mode: "lines+markers",
  type: "line",
};
var humidityTrace = {
  x: [],
  y: [],
  name: "Humidity",
  mode: "lines+markers",
  type: "line",
};
var pressureTrace = {
  x: [],
  y: [],
  name: "Pressure",
  mode: "lines+markers",
  type: "line",
};
var dewPointTrace = {
  x: [],
  y: [],
  name: "Dew Point",
  mode: "lines+markers",
  type: "line",
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

Plotly.newPlot(temperatureHistoryDiv, [temperatureTrace], temperatureLayout);
Plotly.newPlot(humidityHistoryDiv, [humidityTrace], humidityLayout);
Plotly.newPlot(pressureHistoryDiv, [pressureTrace], pressureLayout);
Plotly.newPlot(dewPointHistoryDiv, [dewPointTrace], dewPointLayout);

// Gauge Data
var temperatureData = [
  {
    domain: { x: [0, 1], y: [0, 1] },
    value: 0,
    title: { text: "Temperature" },
    type: "indicator",
    mode: "gauge+number+delta",
    delta: { reference: 30 },
    gauge: {
        axis: { range: [-10, 40] },
      steps: [
        { range: [10, 30], color: "lightgray" },
        { range: [18, 25], color: "gray" },
      ],
      threshold: {
        line: { color: "red", width: 4 },
        thickness: 0.75,
        value: 30,
      },
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
    delta: { reference: 50 },
    gauge: {
      axis: { range: [null, 100] },
      steps: [
        { range: [20, 80], color: "lightgray" },
        { range: [40, 60], color: "gray" },
      ],
      threshold: {
        line: { color: "red", width: 4 },
        thickness: 0.75,
        value: 20,
      },
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
    delta: { reference: 750 },
    gauge: {
      axis: { range: [900, 1100] },
      steps: [
        { range: [950, 1050], color: "lightgray" },
        { range: [980, 1020], color: "gray" },
      ],
      threshold: {
        line: { color: "red", width: 4 },
        thickness: 0.75,
        value: 1050,
      },
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
    delta: { reference: 60 },
    gauge: {
      axis: { range: [-20, 30] },
      steps: [
        { range: [0, 20], color: "lightgray" },
        { range: [8, 15], color: "gray" },
      ],
      threshold: {
        line: { color: "red", width: 4 },
        thickness: 0.75,
        value: 0,
      },
    },
  },
];

var layout = { width: 200, height: 180, margin: { t: 25, b: 0, l: 0, r: 0 } };

Plotly.newPlot(temperatureGaugeDiv, temperatureData, layout);
Plotly.newPlot(humidityGaugeDiv, humidityData, layout);
Plotly.newPlot(pressureGaugeDiv, pressureData, layout);
Plotly.newPlot(dewPointGaugeDiv, dewPointData, layout);

// Will hold the arrays we receive from our BME280 sensor
// Temperature
let newTempXArray = [];
let newTempYArray = [];
// Humidity
let newHumidityXArray = [];
let newHumidityYArray = [];
// Pressure
let newPressureXArray = [];
let newPressureYArray = [];
// dewPoint
let newDewPointXArray = [];
let newDewPointYArray = [];

// The maximum number of data points displayed on our scatter/line graph
let MAX_GRAPH_POINTS = 256;
let ctr = 0;

// Callback function that will retrieve our latest sensor readings and redraw our Gauge with the latest readings
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

      // Update Temperature Line Chart
      updateCharts(
        temperatureHistoryDiv,
        newTempXArray,
        newTempYArray,
        temperature,
        timeStamp
      );
      // Update Humidity Line Chart
      updateCharts(
        humidityHistoryDiv,
        newHumidityXArray,
        newHumidityYArray,
        humidity,
        timeStamp
      );
      // Update Pressure Line Chart
      updateCharts(
        pressureHistoryDiv,
        newPressureXArray,
        newPressureYArray,
        pressure,
        timeStamp
      );

      // Update dewPoint Line Chart
      updateCharts(
        dewPointHistoryDiv,
        newDewPointXArray,
        newDewPointYArray,
        dewPoint,
        timeStamp
      );
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
  var temperature_update = {
    value: temperature,
  };
  var humidity_update = {
    value: humidity,
  };
  var pressure_update = {
    value: pressure,
  };
    var dewPoint_update = {
    value: dewPoint,
  };
  Plotly.update(temperatureGaugeDiv, temperature_update);
  Plotly.update(humidityGaugeDiv, humidity_update);
  Plotly.update(pressureGaugeDiv, pressure_update);
  Plotly.update(dewPointGaugeDiv, dewPoint_update);
}

function updateCharts(lineChartDiv, xArray, yArray, sensorRead, timeStamp) {
  if (xArray.length >= MAX_GRAPH_POINTS) {
    xArray.shift();
  }
  if (yArray.length >= MAX_GRAPH_POINTS) {
    yArray.shift();
  }
  xArray.push(timeStamp);
  yArray.push(sensorRead);

  var data_update = {
    x: [xArray],
    y: [yArray],
  };

  Plotly.update(lineChartDiv, data_update);
}

// Continuos loop that runs evry 3 seconds to update our web page with the latest sensor readings
(function loop() {
  setTimeout(() => {
    updateSensorReadings();
    loop();
  }, 3000);
})();
