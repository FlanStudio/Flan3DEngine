using System.Collections;
using System.Collections.Generic;
using FlanEngine;

public class TestScript : FlanScript
{
    bool firstTime = true;
    List<GameObject> childs = new List<GameObject>();

    //Use this method for initialization
    public override void Awake()
    {
         
    }

    //Called every frame
    public override void Update()
    {
        if(Input.GetKeyDown(KeyCode.KEY_1))
        {
            Debug.Log("Hey! I have pressed " + KeyCode.KEY_1.ToString() + " key!");
            GameObject instance = GameObject.Instantiate();
            childs.Add(instance);
            instance.name = "Child: " + childs.Count.ToString();
        }

        if (Input.GetKeyDown(KeyCode.KEY_2))
        {
            Debug.Log("Hey! I have pressed " + KeyCode.KEY_2.ToString() + " key!");
        }
        if(Input.GetKey(KeyCode.KEY_A))
        {
            Debug.Log("Hey! I have pressed " + KeyCode.KEY_A.ToString() + " key!");
        }


    }
}

