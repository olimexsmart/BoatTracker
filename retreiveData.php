<?php

ini_set('display_errors', 'On');
error_reporting(E_ALL);

require_once 'login.php';
$sql = new mysqli($hostName, $userName, $passWord, $dataBase);
if ($sql->connect_error) {
    die($sql->connect_error);
}
$sql->options(MYSQLI_OPT_INT_AND_FLOAT_NATIVE, TRUE); // Retreives ints as ints and not as strings

$post = file_get_contents('php://input');
$params = json_decode($post, true);
/*
The parameters are:
- start: timestamp start
- end: timestamp end
- sensors: [int array] ids of sensor required
 */

// Loop through years if is that necessary
$startYear = getdate($params['start'])['year'];
$endYear = getdate($params['end'])['year'];
$data = array(null, null);
$i = 0;
$sensors = array("temp", "hum");
foreach ($sensors as &$s) {
    for ($y = $startYear; $y <= $endYear; $y++) {
        // Query data
        $query = "SELECT minute";
        sort($params['sensors']);   // Select each sensor in ascending order
        foreach ($params['sensors'] as &$id) {
            $query .= ",`$s-$id`";
        }
        $query .= " FROM logger.`$y-$s`
                    WHERE minute > {$params['start']} AND minute < {$params['end']}
                    ORDER BY minute ASC;";

        if (!($result = $sql->query($query))) {
            echo "Could not retreive data: " . $sql->error;
        }

        $data[$i] = $result->fetch_all();        
        $i++;
    }
}

// DO SOMETHING WITH IT
echo json_encode($data);

$sql->close();
