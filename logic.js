var map;
var markers = [];
var flightPath;
var infoWindow = null;

// Adapting to window size such that both graphs can be displayed
$("#map").css({ height: $(window).height() * 0.9, width: Math.min($(window).width() * 0.9, 1175) });

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
    $.ajax({
        url: "/tracker/retreiveData.php",
        method: "POST",
        dataType: "text",
        data: JSON.stringify({ start: start, end: end }),
        timeout: 30000,
        success: function (result) {
            var result = JSON.parse(result); // Decompress JSON into Javascript array
            clearMarkers();	// Clear map from previous drawings 
            var bounds = new google.maps.LatLngBounds(); // Object used to set correct zoom level and position
            var path = [];

	    // For each point draw a marker
            result.forEach(point => {
                var coordinates = { lat: point[1], lng: point[2] };
                path.push(coordinates);
                bounds.extend(coordinates);
                marker = new google.maps.Marker({
                    position: coordinates,
                    map: map,
                    //animation: google.maps.Animation.DROP, // Stupid ah hell
                    icon: { // Custom icon
                        path: google.maps.SymbolPath.BACKWARD_CLOSED_ARROW,
                        scale: 3,
                        fillColor: 'yellow',
                        fillOpacity: 0.8,
                        strokeColor: 'red',
                        strokeWeight: 1,
                        rotation: point[4] - 180,
                    },
		    // Extending JS object with custom info to be used in info window 
		    labelInfo: point[0] + '<br/>Knots: ' + point[3]
                });
                 marker.addListener('click', function() {
                        if (infoWindow != null) // Close if an  object was present
                                infoWindow.close();
                        infoWindow = new google.maps.InfoWindow({
                                content: this.labelInfo
                        });

                        infoWindow.open(map, this);
                });

                markers.push(marker);

            });

            flightPath = new google.maps.Polyline({
                path: path,
                geodesic: true,
                strokeColor: '#FF0000',
                strokeOpacity: 1.0,
                strokeWeight: 2
            });

            flightPath.setMap(map);
            map.fitBounds(bounds);
        },
        error: function (result) {
            alert('Error retreiving data from server');
        }
    });
}


function initMap() {
    map = new google.maps.Map(document.getElementById('map'), {
        zoom: 2,
        center: { lat: 0, lng: 0 },
	streetViewControl: false
    });
}

function clearMarkers() {
    if (flightPath)
        flightPath.setMap(null);
    for (var i = 0; i < markers.length; i++) {
        markers[i].setMap(null);
    }
}
