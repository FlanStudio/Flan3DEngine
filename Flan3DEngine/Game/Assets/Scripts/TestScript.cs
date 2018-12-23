using System.Collections;
using System.Collections.Generic;
using FlanEngine;

public class TestScript : FlanScript
{

    public float rotSpeed = 150f;
    public float movSpeed = 50f;

    public GameObject bulletPrefab;

    public Transform BulletSpawningPoint;

    //Use this method for initialization
    public override void Awake()
    {
    
    }

    //Called every frame
    public override void Update()
    {
        if(Input.GetKey(KeyCode.KEY_E))
        {
            transform.rotation.Rotate(Vector3.up, -rotSpeed * Time.deltaTime);
        }

        if (Input.GetKey(KeyCode.KEY_Q))
        {
            transform.rotation.Rotate(Vector3.up, +rotSpeed * Time.deltaTime);
        }

        if(Input.GetMouseButtonDown(MouseKeyCode.MOUSE_LEFT))
        {
            if(bulletPrefab != null && BulletSpawningPoint != null)
            {
                GameObject instantiated = GameObject.Instantiate(bulletPrefab);
                instantiated.transform.position = BulletSpawningPoint.globalPosition;
                instantiated.transform.rotation = BulletSpawningPoint.globalRotation;
            }        
        }

        if (Input.GetKey(KeyCode.KEY_W))
        {
            transform.position += transform.forward * movSpeed * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_A))
        {
            transform.position += transform.right * movSpeed * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_S))
        {
            transform.position -= transform.forward * movSpeed * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.KEY_D))
        {
            transform.position -= transform.right * movSpeed * Time.deltaTime;
        }
    }
}
