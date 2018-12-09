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
            gameObject.transform.position.x -= 50 * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_A))
        {
            gameObject.transform.position.z += 50 * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_S))
        {
            gameObject.transform.position.x += 50 * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_D))
        {
            gameObject.transform.position.z -= 50 * Time.deltaTime;
        }
    }
}

