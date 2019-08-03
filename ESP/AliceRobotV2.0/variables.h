typedef struct{
    String Distance;
    String OriginalTires;
    String AddedTires;
}PoleInfo;

typedef struct{
    PoleInfo Poles[10]; //At least 4 and no more than 10 poles will be required.
    String StartTime;
    String StartDate;
    String RunTime;
    String TiresUsed;
    String TotalPoles;
}RunInfo;
