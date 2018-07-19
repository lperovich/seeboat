using Microsoft.WindowsAzure.Storage;
using Microsoft.WindowsAzure.Storage.Table;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace FeatherBoardToTheCloud
{
  public class TableCloudStorage
    {
        public static CloudStorageAccount cloudStorageAccount = CloudStorageAccount.Parse("DefaultEndpointsProtocol=https;AccountName=tealseeboateeststorage;AccountKey=F5GnHieen8Az50v0oR9cIkf7AywiF1O9vQI53TyNy4GqnpbFmzOrM1ztQ+NWf8ph/GfnQyrXe9usjY/tJBHIRA==;EndpointSuffix=core.windows.net");
        public static CloudTableClient tableClient = cloudStorageAccount.CreateCloudTableClient();

        public CloudTable dummyDataTable = tableClient.GetTableReference("TestSeeboatData");

        public bool DoesTableExists()
        {
            return dummyDataTable.ExistsAsync().Result;
        }
        private void BasicTableOperations(CloudTable table)
        {
            BoatWaterData dataEntry = new BoatWaterData("Boat1", "001")
            {
                DeviceID = 10,
                GPSlat = 53.09,
                GPSlong = -51.09,
                GPShour = 5,
                GPSmin = 3,
                GPSsec = 4,
                GPSms = 2,
                TempVal = 70.05,
                CondVal = 1.2,
                PhVal = 8.00,
                MilliIrradiance = 10.09

            };

            TableOperation.InsertOrMerge(dataEntry);
            dummyDataTable.ExecuteAsync(TableOperation.Insert(dataEntry)); // work until this pt
            TableBatchOperation batchOperation = new TableBatchOperation();
            for (int i = 0; i < 5; i++)
            {
                BoatWaterData dataEntry2 = new BoatWaterData("Boat1", "00" + i)
                {
                    DeviceID = 1 + i,
                    GPSlat = 53.09 + i,
                    GPSlong = -51.09 + i,
                    GPShour = 5 + i,
                    GPSmin = 3 + i,
                    GPSsec = 4 + i,
                    GPSms = 2 + i,
                    TempVal = 70.05 + i,
                    CondVal = 1.2 + i,
                    PhVal = 8.00 + i,
                    MilliIrradiance = 10.09 + i

                };
                batchOperation.InsertOrMerge(dataEntry2);

                dummyDataTable.ExecuteBatchAsync(batchOperation);
               
            }

        }
        public void SendDataToCloud()
        {
            BasicTableOperations(dummyDataTable);

        }
    }

}
