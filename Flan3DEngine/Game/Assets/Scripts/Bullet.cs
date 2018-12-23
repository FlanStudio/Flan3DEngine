using System.Collections;
using FlanEngine;

public class Bullet : FlanScript
{
    public float speed = 50f;
    public float secondsToDestroy = 50f;

    private float timeStarted = 0f;

    //Use this method for initialization
    public override void Awake()
    {

    }

    //Called every frame
    public override void Update()
    {
        transform.position += speed * Time.deltaTime * transform.forward;
    }
}

