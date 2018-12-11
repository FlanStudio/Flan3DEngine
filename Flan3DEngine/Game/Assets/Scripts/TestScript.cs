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
        Debug.ClearConsole();
        if(Camera.main != null)
        {
            Debug.Log("Hey! There is a camera!");
        }
        else
        {
            Debug.Log("Oooooh... There is not a camera :c");
        }

        if (Input.GetKeyDown(KeyCode.KEY_Q))
        {
            Debug.Log("My GameObject's name is " + gameObject.name);
        }
        
        if(Input.GetKeyDown(KeyCode.KEY_1))
        {
            Debug.Log("Instantiating GameObject");
            GameObject instance = GameObject.Instantiate();
            childs.Add(instance);
            instance.name = "Child: " + childs.Count.ToString();
        }

        if (Input.GetKeyDown(KeyCode.KEY_2))
        {        
            if(childs.Count != 0)
            {
                Debug.Log("Deleting GameObject");
                GameObject toDelete = childs[childs.Count-1];              
                Destroy(toDelete);
                childs.Remove(toDelete);
            }
        }

        if (Input.GetKey(KeyCode.KEY_W))
        {
            transform.position += transform.forward * 50 * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_A))
        {
            transform.position += transform.right * 50 * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_S))
        {
            transform.position -= transform.forward * 50 * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_D))
        {
            transform.position -= transform.right * 50 * Time.deltaTime;
        }
    }
}

