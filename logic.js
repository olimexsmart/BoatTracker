var graphs;
// Adapting to window size such that both graphs can be displayed
$("div[id^='graph']").css({ height: $(window).height() * 0.5, width: Math.min($(window).width() * 0.9, 1175) });

$('#today').click(function () {
    var now = Math.round(new Date() / 1000);
    newRequest(now - 86400, now);
});

$('#week').click(function () {
    var now = Math.round(new Date() / 1000);
    newRequest(now - 86400 * 7, now);
});

$('#month').click(function () {
    var now = Math.round(new Date() / 1000);
    newRequest(now - 86400 * 30, now);
});

$('#confirm').click(function () {
    // MANAGE EXCEPTIONS IN PARSING HERE
    start = Math.round(Date.parse($('#start').val()) / 1000);
    end = Math.round(Date.parse($('#end').val()) / 1000);
    if (isNaN(start) || isNaN(end)) {
        alert("Dates badly formatted.");
        return;
    }
    if (start < end)
        newRequest(start, end);
    else
        alert('Start date should be before end date\nCannot warp the space continuum here');
});


////////////////////////////////////////////////////////////////////////////////////////////////

function newRequest(start, end) {
    graphs = [];
    // Get which sensors are checked
    sensors = [];
    sensorNames = ["Date"];
    checked = $(':checked');
    for (let i = 0; i < checked.length; i++) {
        sensors.push(checked[i].id);
        sensorNames.push(checked[i].name);
    }

    $.ajax({
        url: "/logger/retreiveData.php",
        method: "POST",
        dataType: "text",
        data: JSON.stringify({ start: start, end: end, sensors: sensors }),
        timeout: 30000,
        success: function (result) {
            // Create graphs both for temperature and humidity
            result = JSON.parse(result);


            
        },
        error: function (result) {
            alert('Error retreiving data from server');
        }
    });
}




