﻿using System.Collections;
using System.Collections.Generic;
using FlanEngine;

public class TestScript : FlanScript
{
    bool firstTime = true;

    List<GameObject> bullets = new List<GameObject>();

    public float rotSpeed = 150f;
    public float movSpeed = 50f;

    public GameObject bulletPrefab;

    public Transform transformTest;
    public Transform BulletSpawningPoint;

    //Use this method for initialization
    public override void Awake()
    {
    
    }

    //Called every frame
    public override void Update()
    {
        if (transformTest != null)
        {
            Debug.ClearConsole();
            Debug.Log("My transformTest's name is " + transformTest.gameObject.name);
            Debug.Log("My transformTest's position is " + transformTest.position.ToString());
        }

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
            if(bulletPrefab != null)
            {
                Debug.Log("Instantiating GameObject");

                GameObject instantiated = GameObject.Instantiate(bulletPrefab);
                bullets.Add(instantiated);
                instantiated.name = "Child: " + bullets.Count.ToString();

                instantiated.transform.position = BulletSpawningPoint.globalPosition;
                instantiated.transform.rotation = BulletSpawningPoint.globalRotation;
            }        
        }

        if (Input.GetKeyDown(KeyCode.KEY_2))
        {
            if(bullets.Count != 0)
            {
                Debug.Log("Deleting GameObject");
                GameObject toDelete = bullets[bullets.Count - 1];
                Destroy(toDelete);
                bullets.Remove(toDelete);
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
