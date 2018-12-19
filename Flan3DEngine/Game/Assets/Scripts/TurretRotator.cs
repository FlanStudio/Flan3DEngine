using System.Collections;
using System.Collections.Generic;
using FlanEngine;

public class TurretRotator : FlanScript
{
    public GameObject instance;
    List<GameObject> childs = new List<GameObject>();

    //Use this method for initialization
    public override void Awake()
    {
        Debug.Log("Instantiating GameObject");
        GameObject instance = GameObject.Instantiate();
        instance.name = "Awake " + childs.Count.ToString();
    }

    //Called every frame
    public override void Update()
    {
        float mouseDX = Input.GetMouseDeltaPosition().x;
        if(mouseDX != 0)
        {
            transform.rotation.Rotate(Vector3.up, 20 * Time.deltaTime * -mouseDX);
        }
    }

    public override void OnStop()
    {
        Debug.Log("Instantiating GameObject");
        GameObject instance = GameObject.Instantiate();
        instance.name = "onstop " + childs.Count.ToString();
    }
}

