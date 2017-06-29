var sensorData;
var ctx;
var ctx2;

function init(){
  ctx = document.getElementById("myChart");
  ctx2 = document.getElementById("myChart2");
  getSensorData();
  setInterval(getSensorData, 10000)
}

function getSensorData(){
  $.ajax({
    url: '/api/sensor',
    dataType: "json",
    contentType: "application/json",
    success: function(data) {
      sensorData = data;
      rebuildChart(ctx);
      rebuildChart(ctx2);
    },
    errror: function(erro) {console.log(error);}
  });
}

/*
for chart the data looks like :
 * datasets: list of lines
  * lines: dict : label / data
   * data : dict of list of x / y (x can be a date)
*/
function rebuildChart(ctx){
  var data = transformDataForChart(sensorData.objects);

  var myChart = new Chart(ctx, {
    type: 'line',
    data: {
      datasets: data
    },
    options: {
      responsive: true,
/*      title:{
          display:true,
          text:"Plot"
      },*/
      scales: {
          xAxes: [{
              type: "time",
              display: true,
              scaleLabel: {
                  display: true,
                  labelString: 'Date'
              }
          }],
          yAxes: [{
              display: true,
              scaleLabel: {
                  display: true,
                  labelString: 'value'
              }
          }]
      },
      animation: {
        duration: 0,
      }
    }
  });
}

function getMeasuresFromSensor(sensor){
  return _.map(sensor, function(point, index){
    return {
      x: point.time_stamp,
      y: point.value
    };
  })
}

function transformDataForChart(data){
  return _.map(data, function(x){
    return {
      label: x.location,
      data: getMeasuresFromSensor(x.measures)
    };
})
}

window.onload = init;
