using Microsoft.WindowsAzure.Storage.Table;
using System;
using System.Collections.Generic;
using System.Text;


namespace FeatherBoardToTheCloud
{
    class BoatWaterData:TableEntity
    {

        public BoatWaterData(string temppartitionkey, string temprowkey)

        {
            this.PartitionKey = temppartitionkey;
            this.RowKey = temprowkey;
        }
                   

            public BoatWaterData() { }

            public int DeviceID { get; set; }

            public double GPSlat { get; set; }
            public double GPSlong { get; set; }
            public int GPShour { get; set; }
            public int GPSmin { get; set; }
            public int GPSsec { get; set; }
            public int GPSms { get; set; }
            public double TempVal { get; set; }
            public double CondVal { get; set; }
            public double PhVal { get; set; }
            public double MilliIrradiance { get; set; }
    }

    }
        
    
