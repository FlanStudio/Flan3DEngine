﻿using System.Collections;
using System.Collections.Generic;
using FlanEngine;

public class TestScript : FlanScript
{
    public bool firstTime = true;
    List<GameObject> childs = new List<GameObject>();

    public float rotSpeed = 150f;
    public float movSpeed = 50f;

    //Use this method for initialization
    public override void Awake()
    {
         
    }

    //Called every frame
    public override void Update()
    {

        System.Reflection.FieldAttributes fieldAttributes;

        if(Input.GetKey(KeyCode.KEY_E))
        {
            transform.rotation.Rotate(Vector3.up, -rotSpeed * Time.deltaTime);
        }

        if (Input.GetKey(KeyCode.KEY_Q))
        {
            transform.rotation.Rotate(Vector3.up, +rotSpeed * Time.deltaTime);
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

        if (Input.GetMouseButton(MouseKeyCode.MOUSE_LEFT))
        {
            Debug.Log("MOUSE_LEFT");
        }
        if (Input.GetMouseButton(MouseKeyCode.MOUSE_MID))
        {
            Debug.Log("MOUSE_MID");
        }
        if (Input.GetMouseButton(MouseKeyCode.MOUSE_RIGHT))
        {
            Debug.Log("RIGTH");
        }
        if (Input.GetMouseButton(MouseKeyCode.MOUSE_X1))
        {
            Debug.Log("MOUSE_X1");
        }
        if (Input.GetMouseButton(MouseKeyCode.MOUSE_X2))
        {
            Debug.Log("MOUSE_X2");
        }
    }
}

