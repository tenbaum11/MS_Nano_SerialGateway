void readBMP180(double ALTmeters)
{
  double tempC,tempF,absPinMb,absPinHg, p0inMb, p0inHg, a;
  char status;
  status = pressure.startTemperature();
  if (status != 0)
  { 
    delay(status);
    status = pressure.getTemperature(tempC);
    if (status != 0)
    {
      tempF=(9.0/5.0)*tempC+32.0;
      //degInF = tempF;
      //degF = (int)tempF;
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(absPinMb,tempC);
        if (status != 0)
        {
          absPinHg = absPinMb*0.0295333727;
          absHg = (int)absPinHg;
          p0inMb = pressure.sealevel(absPinMb,ALTmeters);
          p0inHg = p0inMb*0.0295333727;
          //baroInHg = p0inHg;
          adjHg = (int)p0inHg;
          a = pressure.altitude(absPinMb,p0inMb);
        } else errorFlag=3;
      } else errorFlag=2;
    } else errorFlag=1;
  } else errorFlag=0;
  
//  printDATA("Temp: ", tempC, "C");
//  printDATA("absP: ", absPinMb, "mb");
//  printDATA("adjP: ", p0inMb, "mb");

  gw.send(msgBTEMP.set(tempF,1));
  gw.send(msgBBARO.set(p0inHg, 1));

}
