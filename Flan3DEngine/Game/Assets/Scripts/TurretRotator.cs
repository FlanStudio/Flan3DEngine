using System.Collections;
using System.Collections.Generic;
using FlanEngine;

public class TurretRotator : FlanScript
{
    //Use this method for initialization
    public override void Awake()
    {
    
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
}

