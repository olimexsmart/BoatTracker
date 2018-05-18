<?php

ini_set('display_errors', 'On');
error_reporting(E_ALL);

require_once 'login.php';

$sql = new mysqli($hostName, $userName, $passWord, $dataBase);
if ($sql->connect_error) {
    die($sql->connect_error);
}

$query = "INSERT INTO boattracker.uomodelfaro VALUES(NULL, {$_GET['lat']}, {$_GET['lon']}, {$_GET['speed']}, {$_GET['course']});";

if (!$sql->query($query)) {
    die("Could not insert into database: " . $sql->error);
}

$sql->close();

echo 'OK';
