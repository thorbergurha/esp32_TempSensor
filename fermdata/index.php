<?php
/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

$servername = "DATABASEHOSTNAME";

// REPLACE with your Database name
$dbname = "DATABASENAME";
// REPLACE with Database user
$username = "DATABASEUSERNAME";
// REPLACE with Database user password
$password = "DATABASEPASSWORD";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "SELECT id, sensor, value1, value2, reading_time FROM SensorData ORDER BY id DESC";
$result = mysqli_query($conn, $sql);

$value1Data = '';
$readingTimeArray = '';


if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_sensor = $row["sensor"];
        $row_value1 = $row["value1"];
        $row_value2 = $row["value2"];
		$row_reading_time = $row["reading_time"];
		
		$i = 0;
		
		$value1Array[] = $row_value1;
		
		if ($i/3 == 0) {
			$value1Data = '"' . $row_value1 . '",' . $value1Data;
			$readingTimeArray = '"' . $row_reading_time . '",' . $readingTimeArray;
		}
		$i++;

    }
    $result->free();
}

?>




<!doctype html>
<html>

<head>
	<meta charset="UTF-8">
	<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.min.js"></script>
	<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
</head>

<body>

    <h1>LALALAL
    <?php echo current($value1Array); ?>
	</h1>
	
	<div class="container">
		<canvas id="myChart"></canvas>
	</div>

	<script>
		let myChart = document.getElementById('myChart').getContext('2d');

		let lineChart = new Chart(myChart, {
			type:'line', 
			data:{
				labels:[<?php echo $readingTimeArray; ?>],
				datasets:[{
					label:'Temperature',
					data:[<?php echo $value1Data; ?>],
					backgroundColor: 'transparent',
					borderColor: 'rgba(255,99,132)',
					boarderWidth: 3
				}]
			},
		});

	</script>
</body>

</html>
