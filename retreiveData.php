<?php

ini_set('display_errors', 'On');
error_reporting(E_ALL);

require_once 'login.php';
$sql = new mysqli($hostName, $userName, $passWord, $dataBase);
if ($sql->connect_error) {
    die($sql->connect_error);
}
$sql->options(MYSQLI_OPT_INT_AND_FLOAT_NATIVE, true); // Retreives ints as ints and not as strings

$post = file_get_contents('php://input');
$params = json_decode($post, true);
/*
The parameters are:
- start: timestamp start
- end: timestamp end

 */

$query = " SELECT *
            FROM boattracker.uomodelfaro
            WHERE timestamp > FROM_UNIXTIME({$params['start']}) AND timestamp < FROM_UNIXTIME({$params['end']})
            GROUP BY lat, lon
            ORDER BY timestamp ASC;";

if (!($result = $sql->query($query))) {
    echo "Could not retreive data: " . $sql->error;
}

// DO SOMETHING WITH IT
echo json_encode($result->fetch_all());

$sql->close();
