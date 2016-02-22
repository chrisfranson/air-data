<!doctype html>
<html>
<head>
    <title>DustDuino Data!</title>
    <link rel="stylesheet" type="text/css" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.2/css/bootstrap.min.css">
</head>
<body>
<?php

    require_once('../config/config.php');

?>
    <table class="data-table table">
        <thead>
            <tr>
                <th>Timestamp</th>
                <th>PM10Conc</th>
                <th>PM25Conc</th>
                <th>PM10Count</th>
                <th>PM25Count</th>
            </tr>
        </thead>
        <tbody>
        </tbody>
    </table>



    <script src="https://code.jquery.com/jquery-1.12.0.min.js"></script>
    <script type="text/javascript">

    $(document).ready(function() {

        var deviceID = "<?php echo $particle_deviceID; ?>";
        var accessToken = "<?php echo $particle_accessToken; ?>";
        var eventSource = new EventSource("https://api.particle.io/v1/devices/" + deviceID + "/events/?access_token=" + accessToken);

        eventSource.addEventListener('open', function(e) {
            console.log("Opened!");
        },false);

        eventSource.addEventListener('error', function(e) {
            console.log("Errored!");
        },false);

        eventSource.addEventListener('Particles', function(e) {
            var rawData = JSON.parse(e.data),
                data = JSON.parse(rawData.data);

            var $dataRow = $(
                '<tr>' +
                    '<td>' + rawData.published_at + '</td>' +
                    '<td>' + data.PM10Conc + '</td>' +
                    '<td>' + data.PM25Conc + '</td>' +
                    '<td>' + data.PM10count + '</td>' +
                    '<td>' + data.PM25count + '</td>' +
                '</tr>'
            );

            $('.data-table tbody').prepend($dataRow);

        }, false);
    });

    </script>
</body>
</html>
