using System.Collections;
using FlanEngine;

public class TestScript : FlanScript
{
    bool firstTime = true;
    GameObject instance;
    //Use this method for initialization
    public override void Awake()
    {
         
    }

    //Called every frame
    public override void Update()
    {
        if (firstTime)
        {
            firstTime = false;
            instance = GameObject.Instantiate();
            instance.name = "MI PUTITO";

            gameObject.name = "MI PAPI";
        }

        if(instance != null)
            Debug.Log("El nombre de mi putito es " + instance.name);

        //Debug.ClearConsole();
        //Debug.Log("The test is working");
    }
}

