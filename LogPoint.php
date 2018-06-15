<?php

ini_set('display_errors', 'On');
error_reporting(E_ALL);

require_once 'login.php';

$sql = new mysqli($hostName, $userName, $passWord, $dataBase);
if ($sql->connect_error) {
    die($sql->connect_error);
}

$buffered = isset($_GET['buffered']) ? 1 : 0;

// Chenge to accomodate date-time from gps
$query = "INSERT INTO boattracker.uomodelfaro VALUES('{$_GET['date']}', {$_GET['lat']}, {$_GET['lng']}, {$_GET['speed']}, {$_GET['course']}, $buffered);";

if (!$sql->query($query)) {
    if (strpos($sql->error, "Duplicate entry") !== false)
        die('ROGER'); // If the entry is already present for any reason, don't block the dump execution
    die("Could not insert into database: " . $sql->error);
}

$sql->close();

echo 'ROGER';
