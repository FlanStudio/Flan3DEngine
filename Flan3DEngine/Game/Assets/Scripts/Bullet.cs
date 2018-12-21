using System.Collections;
using FlanEngine;

public class Bullet : FlanScript
{
    //Use this method for initialization
    public override void Awake()
    {
        Debug.Log("A new Bullet has been instantiated");
    }

    //Called every frame
    public override void Update()
    {
        Debug.Log("Im Updating");
    }
}

