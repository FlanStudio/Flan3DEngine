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
        if(Input.GetKeyDown(Input.KeyCode.KEY_1))
        {
            Debug.Log("Hey! I have pressed " + Input.KeyCode.KEY_1.ToString() + " key!");
        }
        if (Input.GetKeyDown(Input.KeyCode.KEY_2))
        {
            Debug.Log("Hey! I have pressed " + Input.KeyCode.KEY_2.ToString() + " key!");
        }
        if(Input.GetKey(Input.KeyCode.KEY_A))
        {
            Debug.Log("Hey! I have pressed " + Input.KeyCode.KEY_A.ToString() + " key!");
        }
    }
}

