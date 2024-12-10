    /*sprintf(sStr,"LCtrl:%01i",_sKStat.iLCtrl);      gl_write(1, 1,sStr);
    sprintf(sStr,"RCtrl:%01i",_sKStat.iRCtrl);      gl_write(1,10,sStr);
    sprintf(sStr,"LShif:%01i",_sKStat.iLShift);     gl_write(1,20,sStr);
    sprintf(sStr,"RShif:%01i",_sKStat.iRShift);     gl_write(1,30,sStr);
    sprintf(sStr,"LAlt :%01i",_sKStat.iLAlt);       gl_write(1,40,sStr);
    sprintf(sStr,"RAlt :%01i",_sKStat.iRAlt);       gl_write(1,50,sStr);
    sprintf(sStr,"NumLo:%01i",_sKStat.iNumLock);    gl_write(1,60,sStr);
    sprintf(sStr,"CapLo:%01i",_sKStat.iCapsLock);   gl_write(1,70,sStr);
    sprintf(sStr,"ScoLo:%01i",_sKStat.iScrollLock); gl_write(1,80,sStr);
    sprintf(sStr,"OverW:%01i",_sKStat.iOverWrite);  gl_write(1,90,sStr);*/
    
        //Buffers
        SetColor(15);
        sprintf(sStr,"Key Buffer:%02i ",_iKBuffPtr); gl_write(1,1,sStr);
        if(_iKBuffPtr>-1) 
        {
            for(i=0;i<=_iKBuffPtr;i++)
            { 
                sprintf(sStr,"%c ",_iKBuffer[i]);
                gl_write(8*2*i,10,sStr);
            }
        }
        sprintf(sStr,"Con Buffer:%02i ",_iConPtr); gl_write(1,30,sStr);
        if(_iConPtr>-1) 
        {
            for(i=0;i<=_iConPtr;i++)
            { 
                sprintf(sStr,"%c ",_sConBuff[i]);
                gl_write(8*2*i,40,sStr);
            }
        }
